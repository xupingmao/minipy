void DictPrint(TmDict* dict) {
    DictNode *node = dict->head;
    while (node != NULL) {
        printf("[%d:%d]->", node->hash, node->hash % dict->cap);
        tmPrint(node->key);
        printf(":");
        tmPrint(node->val);
        puts("");
        node = node->next;
    }
}
