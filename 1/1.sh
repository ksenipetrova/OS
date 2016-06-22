#!/bin/bash

function isEnd { #проверка на то, что есть 3 вряд
	end=false
	
	sum=0
	for i in 0 1 2
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 3 4 5
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 6 7 8
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 0 3 6
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 1 4 7
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 2 5 8
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 0 4 8
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi

	sum=0
	for i in 2 4 6
	do
		sum=$(($sum+${arr[$i]}))
	done
	if [ $sum == 3 ]; then
		end=true
		return
	fi
	if [ $sum == -3 ]; then
		end=true
		return
	fi
}

function redrawField { # перерисовывет поле
	echo "  1 2 3"
	
	line="a "
	for i in 0 1 2
	do
		if [ ${arr[$i]} == "1" ]; then
			sym="X"
		fi
		if [ ${arr[$i]} == "-1" ]; then
			sym="O"
		fi
		if [ ${arr[$i]} == "0" ]; then
			sym=" "
		fi
		line="$line$sym "
	done
	echo "$line"

	line="b "
	for i in 3 4 5
	do
		if [ ${arr[$i]} == "1" ]; then
			sym="X"
		fi
		if [ ${arr[$i]} == "-1" ]; then
			sym="O"
		fi
		if [ ${arr[$i]} == "0" ]; then
			sym=" "
		fi
		line="$line$sym "
	done
	echo "$line"

	line="c "
	for i in 6 7 8
	do
		if [ ${arr[$i]} == "1" ]; then
			sym="X"
		fi
		if [ ${arr[$i]} == "-1" ]; then
			sym="O"
		fi
		if [ ${arr[$i]} == "0" ]; then
			sym=" "
		fi
		line="$line$sym "
	done
	echo "$line"
}

#start:

p1="/tmp/p1"
p2="/tmp/p2"

if [ "$1" == "--server" ]; then
	crosses=true #играем за крестики
	playing=true #первыми ходим мы
	mknod $p1 p #сервер создает пайпы
	mknod $p2 p
	in=$p1
	out=$p2
else
	crosses=false
	playing=false
	in=$p2
	out=$p1
fi

clear

arr[0]=0 # 0 - пусто, -1 - нолик, 1 - крестик
arr[1]=0
arr[2]=0
arr[3]=0
arr[4]=0
arr[5]=0
arr[6]=0
arr[7]=0
arr[8]=0

redrawField

while [ 1 == 1 ];
do
	if [ $playing == true ]; then
		read -p 'Choose place: ' place #считали ход

		case ${place:0:1} in
		'a')
			line=0
			;;
		'b')
			line=1
			;;
		'c')
			line=2
			;;
		*)
			echo "wrong place"
			continue
			;;
		esac

		case ${place:1:1} in
		'1')
			col=0
			;;
		'2')
			col=1
			;;
		'3')
			col=2
			;;
		*)
			echo "wrong place"
			continue
			;;
		esac

		index=$(($line*3+$col))

		if [ ${arr[$index]} != "0" ]; then
			echo "wrong place"
			continue			
		fi

		if [ $crosses == true ]; then
			arr[$index]=1
		else
			arr[$index]=-1
		fi

		echo -n $place > $out #отправили оппоненту
		playing=false
		
		redrawField		

		isEnd
		if [ $end == true ]; then
			echo "Victory"
			break
		fi

	else
		read place < $in #приняли шаг

		case ${place:0:1} in
		'a')
			line=0
			;;
		'b')
			line=1
			;;
		'c')
			line=2
			;;
		esac

		case ${place:1:1} in
		'1')
			col=0
			;;
		'2')
			col=1
			;;
		'3')
			col=2
			;;
		esac

		index=$(($line*3+$col))
		if [ $crosses == true ]; then
			arr[$index]=-1
		else
			arr[$index]=1
		fi

		playing=true

		redrawField

		isEnd
		if [ $end == true ]; then
			echo "Defeat"
			break
		fi
	fi
done

if [ "$1" == "--server" ]; then
	rm $p1
	rm $p2
fi
