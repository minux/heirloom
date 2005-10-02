#!/sbin/sh

#
# This script uses troff to print the characters and character names of
# one or more PostScript fonts. It accepts the AFM files of the respective
# fonts as arguments, and expects these files to be in the current directory.
# If matching PFB, PFA, or T42 files also exist in the current directory,
# they are included. Alternatively, it prints the characters in an OpenType
# font.
#

# Sccsid @(#)showfont.sh	1.6 (gritter) 10/2/05

pwd=`pwd`

for i
do (
	case $i in
	*.otf|*.ttf)
		supply=$i
		;;
	*)
		base=`expr "$i" : '\(.*\)\.afm'`
		if test -f "$base.pfa"
		then
			supply=$base.pfa
		elif test -f "$base.pfb"
		then
			supply=$base.pfb
		elif test -f "$base.t42"
		then
			supply=$base.t42
		else
			unset supply
		fi
		;;
	esac
	cat <<-!
		.nr PE 10.8i
		.fp 0 X $i $supply
		.ps 10
		.vs 14
		.ta 12pC 24p
		.nr CL 0 1
		.nr PN 1 1
		.de NC
		.	sp |6P
		.	po +8P
		.	if (\\\\n+(CL%5=0) \\{\\
		.		wh \\\\n(PEu
		.		bp
		.		wh \\\\n(PEu NC
		.		po 1i
		.		sp |4P
		\\\\*(FN (Page \\\\n+(PN)
		.		sp |6P
		.	\\}
		..
		.wh \\n(PEu NC
		.sp 6P
	!
	case $i in
	*.otf|*.ttf)
		otfdump -n "$i" | nawk '{
			printf(".ds FN \\fH\\s(12'"$i"' \\(em %s\n", $2)
			print ".mk S"
			print ".sp |4P"
			print "\\*(FN"
			print ".sp |\\nSu"
		}'
		otfdump -c "$i" | nawk '{
			printf("\t\\s(11\\fX\\[%s]\t\\s8\\fH%s\n", $2, $2)
			print ".br"
		}'
		;;
	*)
		nawk <"$i" '
			$1 == "FontName" {
				printf(".ds FN \\fH\\s(12'"$i"' \\(em %s\n", $2)
				print ".mk S"
				print ".sp |4P"
				print "\\*(FN"
				print ".sp |\\nSu"
			}
			$1 == "StartCharMetrics" {
				state = 1
			}
			state == 1 && $1 == "C" && \
					match($0, /(^|;)[ 	]*N[	]*/) {
				name = substr($0, RSTART+RLENGTH+1)
				match(name, /[ 	;]/)
				name = substr(name, 1, RSTART-1)
				printf("\t\\s(11\\fX\\[%s]\t\\s8\\fH%s\n",\
					name, name)
				print ".br"
				n++
			}
			state == 1 && $1 == "EndCharMetrics" {
				state = 0
			}
		'
		;;
	esac
	cat <<-!
		.wh \\n(PEu
	!
   ) | TROFFONTS=$pwd troff -x
done | dpost
