#!/bin/bash

database=
verbose=
aggregate=
timerange=84600
timestart=`date +%s`
timeend=$(($timestart + $timerange))
upper=85
lower=55

###################################
# output script usage information #
###################################

usage() {
    cat << EOF
usage: $0 options

OPTIONS:
   -h      Show this message
   -s      Source name without the extension
   -b      Beginning date (defaults to the current timestamp)
   -e      Ending date (defaults to the beginning date + date range)
   -r      Date range (defaults to 84600 seconds)
   -u      Upper temperature reading (defautls to 85)
   -l      Lower temperature reading (defaults to 55)
   -a      Update unsynchronized timestamps
   -v      Verbose
EOF
}

############################
# process script arguments #
############################

while getopts “ht:d:b:e:r:u:l:at:v” OPTION; do
    case $OPTION in
        h)
            usage
            exit
            ;;
        d)
            database=$OPTARG
            ;;
        b)
            timestart=$OPTARG
            ;;
        e)
            timeend=$OPTARG
            ;;
        r)
            timerange=$OPTARG
            timeend=$(($timestart + $timerange))
            ;;
        u)
            upper=$OPTARG
            ;;
        l)
            lower=$OPTARG
            ;;
        a)
            aggregate=1
            ;;
        v)
            verbose=1
            ;;
    esac
done

#######################
# validate input data #
#######################

if [ -z $database ]; then
    usage
    exit 1
fi

if [ ! -e "$database.log" ]; then
    echo "ERROR: $database.log does not exists" >&2
    exit 1
fi

if [ ! -r "$database.log" ]; then
    echo "ERROR: failed to read $database.log" >&2
    exit 1
fi

###############################
# create databases and graphs #
###############################

rrdtool create "$database.rrd" \
    "DS:temperature:GAUGE:15:${lower}:${upper}" \
    RRA:MIN:0.5:12:2400 \
    RRA:MAX:0.5:12:2400 \
    RRA:AVERAGE:0.5:12:2400 \
    --start $timestart \
    --step 15

for line in $(cat "$database.log" | tr -d '\r'); do
    IFS=":"; declare -a data=($line)
    if [ "${data[0]}" == "temp" ]; then
        if [ ! -z $aggregate ]; then
            ctime="$((${data[1]} + $timestart))"
        else
            ctime="${data[1]}"
        fi
        cvalue="${data[2]}"
        rrdtool update "$database.rrd" "${ctime}:${cvalue}"
    fi
done

rrdtool graph "$database.png" \
    "DEF:probe1=${database}.rrd:temperature:AVERAGE" \
    AREA:probe1#fcbe2c:"Temperature" \
    VDEF:probe1-max=probe1,MAXIMUM \
    VDEF:probe1-avg=probe1,AVERAGE \
    VDEF:probe1-min=probe1,MINIMUM \
    GPRINT:probe1-max:"Maximum\: %2.2lf°" \
    GPRINT:probe1-avg:"Average\: %2.2lf°" \
    GPRINT:probe1-min:"Minimum\: %2.2lf°" \
    --title "Temperature Plot (DS18B20)" \
    --vertical-label "Degrees Farenheit" \
    --width 800 \
    --height 350 \
    --color "BACK#1f1c1e" \
    --color "FONT#ffffff" \
    --color "SHADEA#1f1c1e" \
    --color "SHADEB#1f1c1e" \
    --color "CANVAS#1f1c1e" \
    --color "ARROW#e2233b" \
    --color "AXIS#ffffff50" \
    --upper-limit $upper \
    --lower-limit $lower \
    --start $timestart \
    --end $timeend >/dev/null
