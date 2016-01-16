
for name in __modules__:
    mod = __modules__[name]
    printf("module> %s\n", name)
    printf("mod_dict_size=%s\n", len(mod))
    if "__name__" in mod:
        printf('__name__ = %s\n', mod.__name__)
    print()
