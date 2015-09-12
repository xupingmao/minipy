
for name in __modules__:
    mod = __modules__[name]
    printf('%s.__name__ = %s\nmod_dict_size = %s\n\n', name, mod.__name__, len(mod))
