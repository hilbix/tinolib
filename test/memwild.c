TEST_1(tino_memwildcmp("a\0b\0c", 5, "[z", 2));
TEST1(tino_memwildcmp("a\0b\0c", 5, "*[z]*", 5));
TEST0(tino_memwildcmp("a\0b\0c", 5, "*[b]*", 5));
TEST0(tino_memwildcmp("a\0b\0c", 5, "*b*", 3));
TEST0(tino_memwildcmp("a\0b\0c", 6, "*b*", 4));
TEST0(tino_memwildcmp("a\0b\0c", 6, "a????", 6));
TEST1(tino_memwildcmp("a\0b\0c", 5, "a????", 6));
TEST0(tino_memwildcmp("a\0b\0c", 5, "a*", 2));
