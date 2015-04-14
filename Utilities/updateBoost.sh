#!/usr/bin/env bash

# this script should be run from this directory
# extracted_boost is a path to directory named boost_1_xx_y
extracted_boost=$1

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
script_path=$( cd "$( dirname "$0" )" && pwd )
toplevel_path=$(cd $script_path && git rev-parse --show-toplevel)
if [[ $? -ne 0 ]]; then
  die "Could not find the top-level of a Git repository in \"$script_path\""
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

input_variables=( extracted_boost )
for input_variable in ${input_variables[@]}; do
  if [[ -z $(eval echo "\$$input_variable") ]]; then
    die "\$$input_variable must be set"
  fi
done

snapshot_old_sha=$(git rev-list -n 1 HEAD)

## New shapshot tree ##
snapshot_branch_name="updating-boost"
versioned_boost_path="$script_path/boost/"

git checkout -b "$snapshot_branch_name"

git rm -r "$versioned_boost_path"
git commit -m "Updating boost to version: ${extracted_boost} (step: delete old version)"
# now copy parts of boost which are needed
mkdir $versioned_boost_path
for word in $(cat ${script_path}/boostLibs.txt); do
  echo "Copying $word"
  cp -R "${extracted_boost}boost/$word" "${versioned_boost_path}$word"
done
echo "Copying files in ${extracted_boost}boost"
# cp "${extracted_boost}boost/*" "${versioned_boost_path}" #does not work
find "${extracted_boost}boost/" -maxdepth 1 -type f -exec cp '{}' "${versioned_boost_path}" \; #recursive files without dir structure

git add --all "$versioned_boost_path"

snapshot_new_commit_msg="Updating boost to version: ${extracted_boost}"
snapshot_new_sha=$(
  echo "$snapshot_new_commit_msg" |
  GIT_AUTHOR_NAME="Boost Developers" \
  GIT_AUTHOR_EMAIL="boost@lists.boost.org" \
  git commit --amend -m "$snapshot_new_commit_msg"
)

echo "Created updated branch '$snapshot_branch_name'.  You can now merge it."
