#!/bin/bash

PROJECT_NAME=$1

if [ "$PROJECT_NAME" == "" ];
then
	echo " ------ Exit ------ "
	exit -1;
fi

function change_file_content()
{
	sed -i "s/vsBase/$PROJECT_NAME/g" vsBase.*
	sed -i "s/vsBase/$PROJECT_NAME/g" .gitignore
	find -name "*.cpp" |xargs sed -i "s/vsBase/$PROJECT_NAME/g"
	find -name "*.h" |xargs sed -i "s/vsBase/$PROJECT_NAME/g"
}


function change_file_name()
{
	echo "======= Start ======="
	rm -rf tmp.sh
	# echo "#!/bin/sh" > tmp.sh
	find -name "*vsBase*" >> tmp.sh
	sed "p; s/vsBase/$PROJECT_NAME/" tmp.sh | sed "N; s/\n/ /" > run.sh
	sed -i 's/^/git\ mv\ &/g' run.sh
	chmod a+x run.sh
	echo "======= END ======="
}


### Main ##
change_file_content
change_file_name

./run.sh

rm tmp.sh


