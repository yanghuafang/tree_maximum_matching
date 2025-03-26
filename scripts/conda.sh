CONDA_PATH=""
SHELL_NAME=""
if [[ "$(uname)" == "Darwin" ]]; then
    CONDA_PATH="/opt/homebrew/Caskroom/miniconda/base"
    SHELL_NAME="shell.zsh"
elif [[ "$(uname)" == "Linux" ]]; then
    CONDA_PATH="$HOME/miniconda3"
    SHELL_NAME="shell.bash"
else
    echo "It does not support conda on $(uname)"
    exit 1
fi

echo "Platform: $(uname)"
echo "Conda: $CONDA_PATH"

# >>> conda initialize >>>
# !! Contents within this block are managed by 'conda init' !!
__conda_setup="$("$CONDA_PATH/bin/conda" $SHELL_NAME 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "$CONDA_PATH/etc/profile.d/conda.sh" ]; then
        . "$CONDA_PATH/etc/profile.d/conda.sh"
    else
        export PATH="$CONDA_PATH/bin:$PATH"
    fi
fi
unset __conda_setup
# <<< conda initialize <<<