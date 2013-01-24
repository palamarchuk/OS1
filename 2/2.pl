use sort '_quicksort';
use sort 'stable';
$\ = $/ = undef;
$, = "\n";
die 'Usage: 2.pl output [input1 [input2 [...]]]' if (0 + @ARGV) < 1;
@acc = ();
$output_file = shift;
for (@ARGV)
{
	open FILE, '<', $_ or (warn "cant open file $_ for input\n" and next);
	eval
	{
		$f = <FILE>;
		@m = $f=~/[-+]?\d+/msig;
		@m = map $_ + 0, @m;
		push @acc, @m;
	};
	warn $@ if $@;
	close FILE;
}

eval {@acc = sort {$a<=>$b} @acc;}; die $@ if $@;

open OUTFILE, '>', $output_file or die "cant open file $output_file for output\n";
print OUTFILE @acc; 
close OUTFILE;