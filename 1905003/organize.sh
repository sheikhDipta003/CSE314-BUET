#!/bin/bash

if [[ $# -lt 4 || $# -gt 6 ]];
then
    echo "Usage:"
    echo "./organize.sh <submission folder> <target folder> <test folder> <answer folder> [-v] [-noexecute]"
    echo ""
    echo "-v: verbose"
    echo "-noexecute: do not execute code files"
else
    # iterate over all files in this directory
    find . -wholename "./${1}/*.zip" | while read -r file; do
    
    file2=$(basename "$file")     # submissions/file.zip -> file.zip
    
    # Unzip and store in proper dir
    # C
    dest_dir="./${2}/C/${file2: -11: -4}/"
    mkdir -p "$dest_dir"
    unzip -j "./${1}/$file2" '*.c' -d "$dest_dir" > /dev/null 2>&1

    # Java
    dest_dir="./${2}/Java/${file2: -11: -4}/"
    mkdir -p "$dest_dir"
    unzip -j "./${1}/$file2" '*.java' -d "$dest_dir" > /dev/null 2>&1

    # Python
    dest_dir="./${2}/Python/${file2: -11: -4}/"
    mkdir -p "$dest_dir"
    unzip -j "./${1}/$file2" '*.py' -d "$dest_dir" > /dev/null 2>&1

    done

    find . -type d -empty -delete        # delete all empty files

    # Rename to main.c, Main.java or main.py as appropriate and execute them
    testfiles=("./${3}"/*)
    
    execute_code()
    {
        dn=$(dirname "$1")
        mv "$1" "${dn}/$3" >/dev/null 2>&1
        
        if [[ -n $4 && $4 = "-v" ]];
        then
            echo "Organizing files of ${dn:(-7)}"
        fi
        
        if [[ -z $5 || $5 != "-noexecute" ]];
        then
            if [[ -n $4 && $4 = "-v" ]];
            then
                echo "Executing files of ${dn:(-7)}"
            fi
            
            if [ "$2" = "C" ];
            then
                gcc "${dn}/main.c" -o "${dn}/main.out"
                for(( i=0;i<${#testfiles[@]};i++))
                do
                    "${dn}/main.out" < "${testfiles[$i]}" > "${dn}/out$((i + 1)).txt"
                done
            elif [ "$2" = "Java" ];
            then
                javac "${dn}/Main.java"
                for(( i=0;i<${#testfiles[@]};i++))
                do
                    java -cp "${dn}" Main < "${testfiles[$i]}" > "${dn}/out$((i + 1)).txt"
                done
            elif [ "$2" = "Python" ];
            then
                for(( i=0;i<${#testfiles[@]};i++))
                do
                    python3 "${dn}/main.py" < "${testfiles[$i]}" > "${dn}/out$((i + 1)).txt"
                done
            fi
        fi
    }
    
    if [[ -n ${5} && ${5} = "-v" ]];
    then
        echo "Found ${#testfiles[@]} test files"
    fi

    find . -wholename "./${2}/**/*.c" | while read -r f; do
        execute_code "${f}" "C" "main.c" "${5}" "${6}"
    done

    find . -wholename "./${2}/**/*.java" | while read -r f; do
        execute_code "${f}" "Java" "Main.java" "${5}" "${6}"
    done

    find . -wholename "./${2}/**/*.py" | while read -r f; do
        execute_code "${f}" "Python" "main.py" "${5}" "${6}"
    done

    # Verify output with given answers in mycodes/Workspace/answers/ dir
    # CSV output file
    answers_folder="./${4}"
    verify_output()
    {
        dest=$(dirname "$1")

        answerfiles=("$answers_folder"/*.txt)

        match=0
        mismatch=0

        for f in "${answerfiles[@]}"; do
            f2=$(basename "$f")

            destfile="$dest/out${f2:3}"
            
            if diff -q "$f" "$destfile" > /dev/null 2>&1
            then
                ((match++))
            else
                ((mismatch++))
            fi
        done

        echo "$2,$3,$match,$mismatch" >> "$results"
    }

    if [[ -z ${6} || ${6} != "-noexecute" ]];
    then
        results="./${2}/result.csv"
        echo "student_id,type,matched,mismatched" > "$results"

        # C
        find . -wholename "./${2}/C/**/main.c" | while read -r f; do
            verify_output "${f}" "${f: -14: -7}" "C"
        done

        # Java
        find . -wholename "./${2}/Java/**/Main.java" | while read -r f; do
            verify_output "${f}" "${f: -17: -10}" "Java"
        done

        # Python
        find . -wholename "./${2}/Python/**/main.py" | while read -r f; do
            verify_output "${f}" "${f: -15: -8}" "Python"
        done
    fi
fi
