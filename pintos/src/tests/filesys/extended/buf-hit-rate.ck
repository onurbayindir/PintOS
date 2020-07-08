# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(buf-hit-rate) begin
(buf-hit-rate) create "data"
(buf-hit-rate) open "data"
(buf-hit-rate) write "data"
(buf-hit-rate) flushed cache
(buf-hit-rate) open "data"
(buf-hit-rate) read1
(buf-hit-rate) open "data"
(buf-hit-rate) read2
(buf-hit-rate) check hit_rate2 > hit_rate1
(buf-hit-rate) end
EOF
pass;
