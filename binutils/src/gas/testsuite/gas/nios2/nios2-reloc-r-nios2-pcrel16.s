# Test for pc-relative relocations
.set norelax
.text
	br ext_label
	br ext_label + 16

