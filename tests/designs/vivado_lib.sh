
function check_command
{
	command=$1
	res=$(which $command)
	if [[ -z "$res" ]]; then
		echo "command $command not found!"
		exit -1
	else
		return 0
	fi
}

function run_command
{
	dir="$1"
	command="$2"
	# script = "$3"
	cur_dir="$(pwd)"
	cd "$dir"
	echo "running $command"
	$command
	if [[ "$?" != "0" ]]; then
		echo "command failed"
		return 1
	fi
	cd $cur_dir
	return 0
}

function clean
{
	dir="$1"
	shift
	to_remove="$@"
	cur_dir="$(pwd)"
	cd "$dir"
	if [[ "$?" != "0" ]]; then
                echo "cannot move to $dir"
                return 1
	fi
	rm -rf $to_remove #&> /dev/null
	cd $cur_dir
	return 0
}

