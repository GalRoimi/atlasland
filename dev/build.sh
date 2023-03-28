#!/bin/bash

set -e # exit on error

declare -r STAGE_DIRS="stages\/stage"

verbose()
{
    echo "+ $@"
    eval "$@"
}

print_error()
{
    echo -e "\033[1;91m${1}\033[0Om" 1>&2
}
print_info()
{
    echo -e "\033[1;95m${1}\033[Om"
}

generate_stage()
{
    if [ ! -d "stages/stage${2}/" ]; then
        print_error "Error: \`stage${2}\` does not exist."
        return 1
    fi

    mkdir -p build/stage${1}
    verbose make -C stages/stage${2}/ setup
    cp -r stages/stage${2}/outputs/. build/stage${1}/
    cp stages/stage${2}/welcome.txt build/stage${1}/.welcome
}

generate_docker()
{
    echo "Atlasland generator!"
    local stages=()

    local stage_dirs=$(ls -d ${STAGE_DIRS}* | sed "s/${STAGE_DIRS}//g" | sort -h | sed "s/^/${STAGE_DIRS}/")
    for stage_dir in ${stage_dirs}; do
        local i=${stage_dir#*stages/stage}
        while true; do
            read -p " ..> Do you want to include \`stage${i}\` ($(cat ${stage_dir}/brief.txt)) (y/n)? " choice
            case "$choice" in
                y|Y )
                    stages+=(${i})
                    break;
                    ;;
                n|N )
                    break;
                    ;;
                * )
                    echo " Invalid option!"
                    ;;
            esac
        done
    done

    # verify that there is at least one stage
    if [ ${#stages[@]} -lt 1 ]; then
        return 0
    fi

    for i in ${!stages[@]}; do
        generate_stage $((${i} + 1)) ${stages[${i}]}
    done

    local final_index=$((${#stages[@]} + 1))

    mkdir -p build/stage${final_index}
    verbose make -C stages/final/ setup
    cp -r stages/final/outputs/. build/stage${final_index}/
    cp stages/final/welcome.txt build/stage${final_index}/.welcome

    local users="stage1,123456 "

    for (( i=1; i<${#stages[@]}; i++)); do
        local index=$((${i} + 1))
        local next_stage=${stages[${i} - 1]}

        local user="stage${index}"
        local pass=$(cat stages/stage${next_stage}/password.txt)

        users+="${user},${pass} "
    done

    local user="stage${final_index}"
    local pass=$(cat stages/stage${stages[${#stages[@]}-1]}/password.txt)
    users+="${user},${pass} "

    python3 ./dev/generate_dockerfile.py --users ${users} > build/Dockerfile
}

clean()
{
    for stage_dir in stages/stage*; do
        make -C ${stage_dir} clean
    done

    rm -rf build
}

"$@"
