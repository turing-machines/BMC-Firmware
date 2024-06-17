if tty | grep -q '^/dev/pts'; then
	export PS1='\h \w \$ '
fi
