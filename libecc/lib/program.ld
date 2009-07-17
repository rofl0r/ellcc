SECTIONS
{
    _text = .;
    .text : { * (.text) }
    _data = .;
    .data : { * (.data) }
    _bss = .;
    .bss  : { * (.bss)  }
    _end = .;
}
