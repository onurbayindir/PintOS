# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(buffer-counter) begin
(buffer-counter) create "data"
(buffer-counter) open "data"
(buffer-counter) write "data"
(buffer-counter) open "data"
(buffer-counter) read "data"
(buffer-counter) checking readcnt
(buffer-counter) checking writecnt
(buffer-counter) end
EOF
pass;
