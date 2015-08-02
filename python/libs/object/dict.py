def _dict_update(self, dict):
    # this will update iterator.
    #for key in dict.keys():
        #self[key] = dict[key]
    for key in dict.keys():
        self[key] = dict[key]
    return self

add_obj_method('dict', 'update', _dict_update)