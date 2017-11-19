#! /bin/bash
#close teamviewer sponsored session message (russian)

counter=0
a=$(dpkg -l | grep wmctrl)
b=$(dpkg -l | grep xdotool)

if [[ -n "$a" && -n "$b" ]]
then

    while true
    do
        #must contain the "sponsored session" window name in your locale language
        wmout=$(wmctrl -l | grep "Спонсируемый сеанс")

        if [[ -n "$wmout" && $counter < 2 ]]
        then
            #get current mouse position
            #you can also use this command to get the position of ok button given the resolution
            str=$(xdotool getmouselocation)
            eval x=( $str )
            xArr=${x[0]}
            yArr=${x[1]}
            xArr1=(${xArr//:/ })
            yArr1=(${yArr//:/ })
            x=${xArr1[1]}
            y=${yArr1[1]}
            
            #close window
			#must contain the "sponsored session" window name in your locale language
            wmctrl -a "Спонсируемый сеанс";
            xdotool mousemove 846 457 click 1;
            
            #restablish mouse position
            xdotool mousemove $x $y;
            
            ((counter++))
        else
            #if there's no such window open, set counter to zero, otherwise do nothing
            if [ -z "$wmout" ]
            then
                counter=0;
            fi
        fi
        sleep 5;
    done

fi
