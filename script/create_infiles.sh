#!/bin/bash
#create_infiles.sh

# command: ./create_infiles.sh DFILE CFILE DIR FPD RPF


rand_date (){
	base=1970 # that makes my year period {1970-2020}
	year=$((RANDOM%years+1))
	year=$(($year+base))
	date="$((RANDOM%days+1))-$((RANDOM%months+1))-$year"
	# echo "rand date: $date"

}

make_rec (){
	ran_status="ENTER"
	validexit=-1
	
	if [[ $entercount -eq 10 ]]	## 1 EXIT every 10 enters
	then
		ran_status=EXIT
		validexit=$((RANDOM%7))	## 1/7 chance for valid exit
		entercount=0
	fi	
	
		#####

	if [[ validexit -gt 0 ]]
	then
		temp=${#exist_recs[*]}
		rec=${exist_recs[$((RANDOM%temp))]}
		rec="${rec/ENTER/EXIT}"

	else
		ran_fname=${fnames[$((RANDOM%num_fnames))]}
		ran_lname=${lnames[$((RANDOM%num_lnames))]}
		ran_d=${dnames[$((RANDOM%num_d))]}
		ran_age=$((RANDOM%age))
		rec="$id $ran_status $ran_fname $ran_lname $ran_d $ran_age"
		((id++))
	fi


	if [[ validexit -eq -1 ]]
	then
		exist_recs=("$rec" "${exist_recs[@]}")
		((entercount++))
	
	fi
}

#####################################################
#####################################################
#####################################################

DFile="./$1"
CFile="./$2"
Dir="../$3"
FpD=$4
RpF=$5

days=30
months=12
years=50
age=120
id=0

fNames="mike
rania
Wolter
Alexa
Fern
Constance
Daniella
Connie
Flora
Rhea
Hannah
Esme
Maddie
Jacob
Rafael
Ben
Brandon
Kelly
Oskar
Conor
Jack
Edgar
Gary
Jakob

test
test1"

lNames="Vour
White
Graves
Aguilar
Mccann
Knight
Reyes
Graham
King
Jenkins
Johnson
Wyatt
Moody
Montgomery
Todd
Parker
Reid
Wright
Bradley
1
2"

Status="ENTER
EXIT"
	# READ INTO ARRAY - DECLARE ARRAYS
fnames=($fNames)
lnames=($lNames)
status=($Status)
declare -a dnames
declare -a cnames
declare -a exist_recs

	# COUNT ELEMS
num_fnames=${#fnames[*]}
num_lnames=${#lnames[*]}
num_status=${#status[*]}

#############################################
#			CREATE COUNTRY ARRAY			#
while IFS= read -r line
do
	dnames=("$line" "${dnames[@]}")
done < "$DFile"
num_d=${#dnames[*]}
#############################################


#############################################
#			CREATE DISEASE ARRAY			#
while IFS= read -r line
do
	cnames=("$line" "${cnames[@]}")
done < "$CFile"
num_c=${#cnames[*]}
#############################################


#############################################
#			CREATE DIR 						#
	# check if dir exists and remove it
rm -r $Dir
mkdir -p $Dir
#############################################

### recs stats
entercount=0

#############################################
#			CREATE COUNTRIES DIRS 			#
	# check if dir exists and remove it
count_cdirs=0
# target=$((RANDOM%num_c))
target=19 #MAX 19
while [ $count_cdirs -ne $target ]
do
	name=$Dir/${cnames[$count_cdirs]}
	((count_cdirs++))
	mkdir -p $name
	usingCdir=$name
	txtcounts=0
	while [ $txtcounts -ne $FpD ]
	do
		rand_date
		txtname="$usingCdir/$date.txt"
		touch $txtname
		((txtcounts++))
		reccount=0
		while [ $reccount -ne $RpF ]
		do
			make_rec
			echo "$rec" >> $txtname
			((reccount++))
		done
	done
done
#############################################



##############################################
############ PRACTISE 	RANDOMS				 #
# ran_fname=${fnames[$((RANDOM%num_fnames))]}#
# ran_lname=${lnames[$((RANDOM%num_lnames))]}#
# echo $ran_fname $ran_lname				 #
##############################################
# echo "DFiles: $DFile"

# rand_date

unset fnames