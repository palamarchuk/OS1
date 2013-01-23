use sort '_quicksort';
use sort 'stable';
$\=$/=undef;
die 'no output file' if (0+@ARGV)<1;
@acc=();
for (@ARGV)
{
	open FILE,'<',$_ or die "cant open file	$_\n";
	$f=<FILE>;
	@m = $f=~/[-+]?\d+/msig;
	@m = map $_+0,@m;
	close FILE;
	push @acc,@m;
}
print sort {$a<=>$b} @m;