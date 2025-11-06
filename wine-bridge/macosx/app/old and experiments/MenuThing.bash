#!/bin/bash
cachefile="$HOME/.MenuThing"

if [ $# -eq 0 ]
then


	if [ -f "$HOME/.MenuThing" ]
	then
		echo cat "$HOME/.MenuThing"
	else

		#p=$(osascript -e 'POSIX path of (choose file )')
		#echo "Found: $p" > "$cachefile"
		#find -L "$p" \
		#\( -path "*/bin/wine" -or -path "*/CrossOver/*" \)
		#/Applications/Wine*.app "$HOME"/Applications/Kegworks/*.app "$HOME"/Applications/Sikarugir/*.app "$HOME"/Applications/CrossOver*.app \
		wines=("Wine")
		crossovers=("CrossOver")
		find /Applications/CrossOver*.app /Applications/Wine*.app "$HOME"/Applications/Kegworks/*.app "$HOME"/Applications/Sikarugir/*.app "$HOME"/Applications/CrossOver*.app \
			 -type f -name "wine" \( -path "*bin/wine" -or -path "*CrossOver*" \) \
			 -print0 2>/dev/null \
			  | ( while IFS= read -r -d '' w
			do
				if [[ "$w" =~ CrossOver ]];
				then
					# echo "CrossOver: $w"
					crossovers+=( "$w" )
				else
					wines+=( "$w" )
					# echo "Wine: $w"
				fi

			done

				echo -n "SUBMENU"
				printf '|%s' "${wines[@]}"
				echo
				echo -n "SUBMENU"
				printf '|%s' "${crossovers[@]}"
				echo
			) # Diese Klammer ist wichtig für die variablen

			# IFS=$'\n' read -rd '' -a linesArray <<< "$wines"
			# for x in "${wines[@]}"; do

			# done

			# echo -n "SUBENU|"
			# printf '%s|' "${wines[@]}"
			# echo -n "SUBENU|"
			# printf '%s|' "${crossovers[@]}"

			# > "$cachefile"
	fi

echo "Reload"
echo "SUBMENU|Title|Item1|Item2|Item3"


else
	case "$1" in
	Reload)
		rm "$HOME/.MenuThing"
		;;
	*)
		say $("$1" --version | head -1)
		# "$1"
		;;
	esac
fi
