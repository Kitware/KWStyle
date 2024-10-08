#!/usr/bin/env bash

if [[ $# -lt 2 || "$1" = "-h" || "$1" = "--help" ]]; then
  echo "Usage: ${0##*/} <extracted_boost> <boost_date_time>"
  echo ''
  echo 'Where:'
  echo ''
  echo '<extracted_boost> is a path to directory named with this pattern:'
  echo '  /home/me/boost_1_xx_y/ (including the trailing slash)!'
  echo '  It can be downloaded from http://www.boost.org/users/download/'
  echo ''
  echo '<boost_date_time> is the date the given version of boost was released,'
  echo '  for example, "2015-04-17T00:00:00"'
  exit 1
fi

# quit function
die()
{
  echo "Error: $@" 1>&2
  exit 1
}

## Validate ##
required_commands=( git grep dirname basename cat )
for required_command in ${required_commands[@]}; do
  if ! command -v $required_command &> /dev/null; then
      die "Command \"$required_command\" not found"
  fi
done

extracted_boost=$1
boost_version=$(basename "$extracted_boost")
boost_date_time=$2

## Set up paths ##
script_path=$( cd "$( dirname "$0" )" && pwd )
toplevel_path=$(cd $script_path && git rev-parse --show-toplevel)
if [[ $? -ne 0 ]]; then
  die "Could not find the top-level of a Git repository in \"$script_path\""
fi
cd "$toplevel_path"


input_variables=( extracted_boost )
for input_variable in ${input_variables[@]}; do
  if [[ -z $(eval echo "\$$input_variable") ]]; then
    die "\$$input_variable must be set"
  fi
done

## Old snapshot commit ##
snapshot_regex="boost_[0-9]_[0-9][0-9]_[0-9]"
if [[ "$extracted_boost" =~ "$snapshot_regex" ]]; then
  die "The folder must be named with this pattern: $snapshot_regex"
fi
snapshot_old_sha=$(git rev-list --grep="$snapshot_regex" -n 1 HEAD)
if [[ -z "$snapshot_old_sha" ]]; then
  die "Old snapshot commit not detected"
fi
echo "Old snapshot commit: $snapshot_old_sha"

## New shapshot tree ##
snapshot_branch_name="updating-boost"
snapshot_temp_dir="$snapshot_branch_name"
snapshot_temp_path="$toplevel_path/$snapshot_temp_dir/"
snapshot_temp_index="$toplevel_path/$snapshot_branch_name.index"
versioned_boost_path="$script_path/boost/"

rm -rf "$snapshot_temp_path" "$snapshot_temp_index"

# now copy parts of boost which are needed
# boostLibs.txt is a list of xpressive's dependencies (only xpressive is used by KWStyle)
mkdir $snapshot_temp_path
for word in $(cat ${script_path}/boostLibs.txt); do
  echo "Copying $word"
  cp -R "${extracted_boost}boost/$word" "${snapshot_temp_path}$word"
done
echo "Copying files in ${extracted_boost}boost"
find "${extracted_boost}boost/" -maxdepth 1 -type f -exec cp '{}' "${snapshot_temp_path}" \;

snapshot_new_tree=$(
  GIT_WORK_TREE="$snapshot_temp_path" &&
  GIT_INDEX_FILE="$snapshot_temp_index" &&
  export GIT_WORK_TREE GIT_INDEX_FILE &&
  git add --all &&
  git write-tree
)

rm -rf "$snapshot_temp_path" "$snapshot_temp_index"

snapshot_new_change_id=$(git commit-tree $snapshot_new_tree -p $snapshot_old_sha </dev/null)
snapshot_new_commit_msg="ENH: Updating boost to version: ${boost_version}"

snapshot_new_sha=$(
    echo "$snapshot_new_commit_msg" |
    GIT_AUTHOR_NAME="Boost Developers" \
    GIT_AUTHOR_EMAIL="boost@lists.boost.org" \
    GIT_AUTHOR_DATE="$boost_date_time" \
    git commit-tree $snapshot_new_tree -p $snapshot_old_sha
  )

git update-ref refs/heads/$snapshot_branch_name $snapshot_new_sha
echo "Created updated branch '$snapshot_branch_name'.  You can now merge it using command:

    cd \"$toplevel_path\" &&
    git merge -X subtree=Utilities/boost $snapshot_branch_name
"
