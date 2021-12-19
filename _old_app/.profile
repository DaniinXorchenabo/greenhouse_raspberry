# ~/.profile: executed by the command interpreter for login shells.
# This file is not read by bash(1), if ~/.bash_profile or ~/.bash_login
# exists.
# see /usr/share/doc/bash/examples/startup-files for examples.
# the files are located in the bash-doc package.

# the default umask is set in /etc/profile; for setting the umask
# for ssh logins, install and configure the libpam-umask package.
#umask 022

# if running bash
echo "prestart"

if [ -n "$BASH_VERSION" ]; then
    # include .bashrc if it exists
    echo "fix 1"
    if [ -f "$HOME/.bashrc" ]; then
	
	if [ -n "$1" ]; then
		echo "fix 2"		
	else
		#если есть аргумент (если запустили из "$HOME/.bashrc")
		echo "fix 2-3"
	        $HOME/.bashrc no
		
		
	fi
    fi
    echo "fix 3"
fi

# set PATH so it includes user's private bin if it exists
echo "pre_path"
if [ -d "$HOME/bin" ] ; then
    PATH="$HOME/bin:$PATH"
    echo "fix path"
fi

export WORKON_HOME=$HOME/.virtualenvs
export VIRTUALENVWRAPPER_PYTHON=/usr/bin/python3
source /usr/local/bin/virtualenvwrapper.sh
echo "end"
