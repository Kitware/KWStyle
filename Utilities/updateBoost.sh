#!/usr/bin/env bash

# this script should be run from this directory
# extracted_boost is a path to directory named boost_1_xx_y
extracted_boost=$1
snapshot_author_name=$2
snapshot_author_email=$3

# quit function
die()
{
  echo "Error: $@" 1>&2
  exit 1
}

if [ ! -d "${extracted_boost}" ]; then
  die "A path to directory named with this pattern: /home/me/boost_1_xx_y/ has to be provided as first argument on the command line (including the trailing slash)!"
fi

## Set up paths ##
module_path=$( cd "$( dirname "$0" )" && pwd )
toplevel_path=$(cd $module_path && git rev-parse --show-toplevel)
if [[ $? -ne 0 ]]; then
  die "Could not find the top-level of a Git repository in \"$module_path\""
fi
cd "$toplevel_path"


## Validate ##
required_commands=( git grep sed egrep tar dirname basename cat )
for required_command in ${required_commands[@]}; do
  $required_command --version >/dev/null 2>&1
  if [[ $? -ne 0 ]]; then
    die "Command \"$required_command\" not found"
  fi
done

input_variables=( extracted_boost snapshot_author_name snapshot_author_email )
for input_variable in ${input_variables[@]}; do
  if [[ -z $(eval echo "\$$input_variable") ]]; then
    die "\$$input_variable must be set"
  fi
done

snapshot_old_sha=$(git rev-list -n 1 HEAD)

## New shapshot tree ##
snapshot_branch_name="updating-boost"
snapshot_temp_path="$toplevel_path/$snapshot_branch_name/"
snapshot_temp_index="$toplevel_path/$snapshot_branch_name.index"

# now copy parts of boost which are needed
mkdir $snapshot_temp_path
for word in $(cat ${module_path}/boostLibs.txt); do
  echo "Copying $word"
  cp -R "${extracted_boost}boost/$word" "${snapshot_temp_path}$word"
done
echo "Copying files in ${extracted_boost}boost"
# cp "${extracted_boost}boost/*" "${snapshot_temp_path}" #does not work
find "${extracted_boost}boost/" -maxdepth 1 -type f -exec cp '{}' "${snapshot_temp_path}" \; #recursive files without dir structure

snapshot_new_tree=$(
  GIT_WORK_TREE="$snapshot_temp_path" &&
  GIT_INDEX_FILE="$snapshot_temp_index" &&
  export GIT_WORK_TREE GIT_INDEX_FILE &&
  git add --all &&
  git write-tree
)

# die "quitting test"
rm -rf "$snapshot_temp_path" "$snapshot_temp_index"

## New shapshot commit ##
snapshot_new_change_id=$(git commit-tree $snapshot_new_tree -p $snapshot_old_sha </dev/null)
snapshot_new_commit_msg="Updating booost to version: ${extracted_boost}

Change-Id: I$snapshot_new_change_id"
snapshot_new_sha=$(
  echo "$snapshot_new_commit_msg" |
  GIT_AUTHOR_NAME="$snapshot_author_name" \
  GIT_AUTHOR_EMAIL="$snapshot_author_email" \
  GIT_AUTHOR_DATE="$upstream_new_datetime" \
  git commit-tree $snapshot_new_tree -p $snapshot_old_sha
)

## New shapshot branch ##
git update-ref refs/heads/$snapshot_branch_name $snapshot_new_sha
module_relative_path=${module_path#"$toplevel_path/"}
echo "Created updated branch '$snapshot_branch_name'.  Merge with command:

    cd \"$toplevel_path\" &&
    git merge -X subtree=$module_relative_path/$extracted_boost $snapshot_branch_name
"
