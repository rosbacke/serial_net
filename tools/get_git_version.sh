#!/bin/bash

GIT_DIR=$1
TARGET=$2
GIT_STATUS=$(git -C ${GIT_DIR} status -s)
#echo ${GIT_STATUS}

if [ -z "${GIT_STATUS}" ]; 
then
    STATUS="clean"
else
    STATUS="dirty"
fi
#echo ${STATUS}

SHORT_HASH=$(git -C ${GIT_DIR} rev-parse --short=8 HEAD)

VER_STRING=${SHORT_HASH}-${STATUS}
echo ${VER_STRING}

TMP1="#ifndef VERSION_STRING_H_\n#define VERSION_STRING_H_\n\n#define VERSION_STRING \""
TMP2="\"\n\n#endif\n"

NEW_CONTENT=$(echo -ne ${TMP1}${VER_STRING}${TMP2})

#echo -ne "New content:${NEW_CONTENT}"

if [ -f ${TARGET} ];
then
    echo "Target exists."
    OLD_CONTENT="$(< ${TARGET})"
#    echo -e "Old content:${OLD_CONTENT}"

    if [ "${OLD_CONTENT}" != "${NEW_CONTENT}" ];
    then
        echo "Difference."
#	echo -ne "${OLD_CONTENT}"
#	echo -ne "${NEW_CONTENT}"
    else
        echo "no difference. Do nothing."
	exit 0
    fi
fi

echo -ne "${NEW_CONTENT}" > ${TARGET}
