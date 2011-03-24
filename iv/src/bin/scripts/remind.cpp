: remind - post reminders for today from calendar file

Remind=remind
#ifdef sgi
CalendarHelper=/usr/lib/calprog
#else
CalendarHelper=/usr/lib/calendar
#endif
MacrosFile=REMIND_MACROS
RemindersFile=$HOME/reminders
AltRemindersFile=$HOME/calendar
NroffPrefix='.po 0;.ll 50;.pl 50;.nh;.na;'
NroffPostfix=';.br;.pl 1'
TrMapIn=';\001\002\003'				# ;  ^A ^B ^C
TrMapOut='\012\047;\134'			# CR '  ;  \

: ${ALERTER=alert}; export ALERTER
: ${DISPLAY=:0}; export DISPLAY
export TESTREMIND
SHELL=/bin/sh; export SHELL

if test ! -f $RemindersFile
then
    RemindersFile=$AltRemindersFile
fi

case $# in

0)  set `date`; day=$1; tmp=/tmp/rem$$

$CalendarHelper | awk '
    NR==1 || NR==2 { print }
    END { print "([Ee]very[	 ]*)(" "'"$day"'" "|day)" }
' > $tmp

cat $MacrosFile $RemindersFile \
    | /lib/cpp \
    | egrep -f $tmp \
    | awk '
	/[0-9][0-9]?:[0-9][0-9]?/ {
	    for ( i=1; i<=NF; i++ ) {
		if ( $i ~ /^[0-9][0-9]?:[0-9][0-9]/ ) {
		    split( $i, time, ":");
		    suffix = substr( time[2], 3, 2 );
		    time[2] = substr( time[2], 1, 2 );
		    if ( suffix == "pm" || substr( $(i+1), 1, 2 ) == "pm" ) {
			time[1] = time[1] + 12;
		    }
		    if ( time[1] < 10 ) {
			time[1] = "0" time[1];
		    }
		    print "echo '\''" $0 "'\'' | '$Remind' " time[1] time[2];
		    break;
		}
	    }
	} '\
    | sh
    
rm -f $tmp ;;

1)  read input
    input="'$input'"
    while read line
	do input="$input '$line'"
    done
    if test $1 -eq 0 -o ${TESTREMIND-no}test != notest
    then DO="sh"
    else DO="at $1"
    fi
#ifdef sgi
    echo " echo '$NroffPrefix'$input'$NroffPostfix' | tr ';' '\\012' | nroff | $ALERTER " | $DO
    ;;
#else
    tmp=/tmp/$$
    echo " \
	echo '$NroffPrefix'$input'$NroffPostfix' \
	| tr '$TrMapIn' '$TrMapOut' \
	| nroff \
	| $ALERTER \
    " > $tmp
    $DO $tmp
    rm -f $tmp ;;
#endif

2)  echo $2 | $Remind $1 ;;

*)  echo "usage : $0 [time] [message]" ;;

esac
