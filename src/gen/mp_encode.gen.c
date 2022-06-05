const char* mp_encode_bin="31\n2#string\n32\n2#number\n32\n2#None\n32\n26##0\n67#mp_encode\n"
"63#11\n2#tm\n28#globals\n48\n17\n10\n37#6\n"
"63#12\n2#boot\n2#*\n1#2\n35#1\n"
"63#14\n2#mp_parse\n2#*\n1#2\n"
"63#15\n2#mp_opcode\n2#*\n1#2\n"
"63#17\n24\n26#_asm_ctx\n"
"63#18\n24\n26#_code_list\n"
"63#19\n24\n26#_ext_code_list\n"
"63#21\n31\n28#OP_JUMP_ON_FALSE\n32\n28#OP_JUMP_ON_TRUE\n32\n28#OP_POP_JUMP_ON_FALSE\n32\n28#OP_SETJUMP\n32\n28#OP_JUMP\n32\n28#OP_NEXT\n32\n26#_jmp_list\n"
"63#31\n33\n2#+\n28#OP_ADD\n34\n2#-\n28#OP_SUB\n34\n2#*\n28#OP_MUL\n34\n2#/\n28#OP_DIV\n34\n2#%\n28#OP_MOD\n34\n2#>\n28#OP_GT\n34\n2#<\n28#OP_LT\n34\n2#>=\n28#OP_GTEQ\n34\n2#<=\n28#OP_LTEQ\n34\n2#==\n28#OP_EQEQ\n34\n2#is\n28#OP_EQEQ\n34\n2#!=\n28#OP_NOTEQ\n34\n2#get\n28#OP_GET\n34\n26#_op_dict\n"
"63#47\n33\n2#+=\n28#OP_ADD\n34\n2#-=\n28#OP_SUB\n34\n2#*=\n28#OP_MUL\n34\n2#/=\n28#OP_DIV\n34\n2#%=\n28#OP_MOD\n34\n26#_op_ext_dict\n"
"63#55\n31\n3#-1.0\n32\n26#_begin_tag_list\n"
"63#56\n31\n3#-1.0\n32\n26#_end_tag_list\n"
"63#58\n3\n26#_tag_cnt\n"
"63#59\n3\n26#_global_index\n"
"63#62\n51#init_pop_value_type_set\n"
"63#62\n57\n"
"63#63\n28#set\n31\n2#call\n32\n48#1\n25\n28#_op_dict\n45\n44#10\n25#1\n"
"63#65\n27\n2#add\n22\n27#1\n48#1\n30\n36#9\n30\n"
"63#66\n27\n52\n53\n26#init_pop_value_type_set\n"
"63#68\n28#init_pop_value_type_set\n48\n26#POP_VALUE_TYPE_SET\n"
"63#70\n54#Scope\n51#__init__\n"
"63#71\n57#256\n"
"63#72\n31\n27\n2#locals\n21\n"
"63#73\n31\n27\n2#globals\n21\n"
"63#74\n31\n27\n2#temp_vars\n21\n"
"63#75\n3\n27\n2#jmps\n21\n53\n28#Scope\n2#__init__\n21\n51#add_global\n"
"63#77\n57#512\n"
"63#78\n27#1\n27\n2#globals\n22\n17\n10\n37#11\n"
"63#79\n27\n2#globals\n22\n2#append\n22\n27#1\n48#1\n30\n35#1\n53\n28#Scope\n2#add_global\n21\n51#get_new_temp\n"
"63#81\n57#256\n"
"63#82\n28#len\n27\n2#temp_vars\n22\n48#1\n25#1\n"
"63#83\n2#%\n28#str\n27#1\n48#1\n4\n25#2\n"
"63#84\n27\n2#temp_vars\n22\n2#append\n22\n27#2\n48#1\n30\n"
"63#85\n24\n52\n53\n28#Scope\n2#get_new_temp\n21\n56\n"
"63#87\n54#AsmContext\n51#__init__\n"
"63#88\n57#256\n"
"63#89\n28#Scope\n48\n27\n2#scope\n21\n"
"63#90\n31\n27\n2#scope\n22\n32\n27\n2#scopes\n21\n53\n28#AsmContext\n2#__init__\n21\n51#push\n"
"63#92\n57#256\n"
"63#93\n28#Scope\n48\n27\n2#scope\n21\n"
"63#94\n27\n2#scopes\n22\n2#append\n22\n27\n2#scope\n22\n48#1\n30\n53\n28#AsmContext\n2#push\n21\n51#pop\n"
"63#96\n57#256\n"
"63#97\n27\n2#scopes\n22\n2#pop\n22\n48\n30\n53\n28#AsmContext\n2#pop\n21\n51#add_local\n"
"63#99\n57#512\n"
"63#100\n27#1\n2#val\n22\n27\n2#scope\n22\n2#locals\n22\n17\n10\n37#15\n"
"63#101\n27\n2#scope\n22\n2#locals\n22\n2#append\n22\n27#1\n2#val\n22\n48#1\n30\n35#1\n53\n28#AsmContext\n2#add_local\n21\n51#load\n"
"63#103\n57#512\n"
"63#105\n28#len\n27\n2#scopes\n22\n48#1\n3#1.0\n15\n37#10\n"
"63#106\n28#emit\n28#OP_LOAD_GLOBAL\n27#1\n2#val\n22\n48#2\n30\n35#40\n27#1\n2#val\n22\n27\n2#scope\n22\n2#locals\n22\n17\n10\n37#10\n"
"63#109\n28#emit\n28#OP_LOAD_GLOBAL\n27#1\n2#val\n22\n48#2\n30\n35#20\n"
"63#111\n27\n2#scope\n22\n2#locals\n22\n2#index\n22\n27#1\n2#val\n22\n48#1\n25#2\n"
"63#112\n28#emit\n28#OP_LOAD_LOCAL\n27#2\n48#2\n30\n53\n28#AsmContext\n2#load\n21\n51#index_local\n"
"63#114\n57#512\n"
"63#115\n27#1\n2#val\n22\n27\n2#scope\n22\n2#locals\n22\n17\n10\n37#15\n"
"63#116\n27\n2#scope\n22\n2#locals\n22\n2#append\n22\n27#1\n2#val\n22\n48#1\n30\n35#1\n"
"63#117\n27\n2#scope\n22\n2#locals\n22\n2#index\n22\n27#1\n2#val\n22\n49#1\n52\n53\n28#AsmContext\n2#index_local\n21\n51#store\n"
"63#119\n57#512\n"
"63#121\n28#len\n27\n2#scopes\n22\n48#1\n3#1.0\n15\n37#10\n"
"63#122\n28#emit\n28#OP_STORE_GLOBAL\n27#1\n2#val\n22\n48#2\n30\n35#34\n27#1\n2#val\n22\n27\n2#scope\n22\n2#globals\n22\n17\n10\n37#15\n"
"63#126\n27\n2#index_local\n22\n27#1\n48#1\n25#2\n"
"63#127\n28#emit\n28#OP_STORE_LOCAL\n27#2\n48#2\n30\n35#9\n"
"63#129\n28#emit\n28#OP_STORE_GLOBAL\n27#1\n2#val\n22\n48#2\n30\n53\n28#AsmContext\n2#store\n21\n56\n"
"63#131\n51#asm_init\n"
"63#131\n57\n"
"63#132\n"
"63#133\n"
"63#134\n"
"63#136\n28#AsmContext\n48\n26#_asm_ctx\n"
"63#137\n31\n26#_code_list\n"
"63#138\n31\n26#_ext_code_list\n53\n26#asm_init\n"
"63#140\n51#chk_try_block\n"
"63#140\n57#256\n"
"63#141\n28#_asm_ctx\n2#scope\n22\n2#jmps\n22\n3\n11\n37#5\n"
"63#142\n3\n52\n35#1\n"
"63#143\n28#_asm_ctx\n2#scope\n22\n2#jmps\n22\n3#1.0\n4\n28#_asm_ctx\n2#scope\n22\n2#jmps\n21\n"
"63#144\n28#emit\n28#OP_SETJUMP\n27\n48#2\n30\n"
"63#145\n3#1.0\n52\n53\n26#chk_try_block\n"
"63#147\n51#exit_try_block\n"
"63#147\n57\n"
"63#148\n28#_asm_ctx\n2#scope\n22\n2#jmps\n22\n3#1.0\n5\n28#_asm_ctx\n2#scope\n22\n2#jmps\n21\n53\n26#exit_try_block\n"
"63#150\n51#asm_switch__code_list\n"
"63#150\n57\n"
"63#151\n"
"63#152\n28#_ext_code_list\n28#_code_list\n41#2\n26#_code_list\n26#_ext_code_list\n53\n26#asm_switch__code_list\n"
"63#155\n51#asm_get_regs\n"
"63#155\n57\n"
"63#156\n28#len\n28#_asm_ctx\n2#scope\n22\n2#locals\n22\n49#1\n52\n53\n26#asm_get_regs\n"
"63#158\n51#store_global\n"
"63#158\n57#256\n"
"63#159\n28#emit\n28#OP_STORE_GLOBAL\n27\n2#val\n22\n48#2\n30\n53\n26#store_global\n"
"63#161\n51#add_global\n"
"63#161\n57#256\n"
"63#162\n28#_asm_ctx\n2#scope\n22\n2#globals\n22\n2#append\n22\n27\n2#val\n22\n48#1\n30\n53\n26#add_global\n"
"63#165\n51#emit\n3\n25#1\n"
"63#165\n57#257\n"
"63#166\n31\n27\n32\n27#1\n32\n25#2\n"
"63#167\n28#_code_list\n2#append\n22\n27#2\n48#1\n30\n"
"63#168\n27#2\n52\n53\n26#emit\n"
"63#170\n51#code_pop\n"
"63#170\n57\n"
"63#171\n28#_code_list\n2#pop\n22\n49\n52\n53\n26#code_pop\n"
"63#173\n51#emit_def\n"
"63#173\n57#256\n"
"63#174\n28#emit\n28#OP_DEF\n27\n2#val\n22\n48#2\n30\n53\n26#emit_def\n"
"63#180\n51#emit_load\n"
"63#180\n57#256\n"
"63#181\n27\n24\n15\n37#7\n"
"63#182\n28#emit\n28#OP_NONE\n49#1\n52\n35#1\n"
"63#184\n27\n2#type\n22\n25#1\n"
"63#185\n27#1\n2#string\n15\n37#10\n"
"63#186\n28#emit\n28#OP_STRING\n27\n2#val\n22\n48#2\n30\n35#48\n27#1\n2#number\n15\n37#10\n"
"63#188\n28#emit\n28#OP_NUMBER\n27\n2#val\n22\n48#2\n30\n35#35\n27#1\n2#None\n15\n37#8\n"
"63#190\n28#emit\n28#OP_NONE\n3\n48#2\n30\n35#24\n27#1\n2#name\n15\n37#9\n"
"63#192\n28#_asm_ctx\n2#load\n22\n27\n48#1\n30\n35#12\n"
"63#194\n28#print\n2#LOAD_LOCAL \n28#str\n27\n2#val\n22\n48#1\n4\n48#1\n30\n53\n26#emit_load\n"
"63#197\n51#find_label\n"
"63#197\n57#512\n"
"63#198\n3\n25#2\n27\n45\n44#34\n25#3\n"
"63#200\n27#3\n3\n22\n28#OP_TAG\n15\n38#7\n27#3\n3#1.0\n22\n27#1\n15\n19\n37#5\n"
"63#202\n27#2\n52\n35#1\n"
"63#203\n27#3\n3\n22\n28#OP_TAG\n16\n37#7\n"
"63#204\n27#2\n3#1.0\n4\n25#2\n35#1\n36#33\n30\n53\n26#find_label\n"
"63#206\n51#resolve_labels\n"
"63#206\n57#256\n"
"63#207\n28#len\n27\n48#1\n25#1\n"
"63#208\n3\n25#2\n"
"63#209\n31\n25#3\n27\n45\n44#63\n25#4\n"
"63#211\n27#4\n3\n22\n28#_jmp_list\n17\n37#34\n"
"63#212\n28#find_label\n27\n27#4\n3#1.0\n22\n48#2\n25#5\n"
"63#213\n27#5\n27#2\n5\n25#6\n"
"63#214\n27#6\n3\n12\n37#10\n"
"63#214\n31\n28#OP_UP_JUMP\n32\n27#6\n9\n32\n25#4\n35#6\n"
"63#215\n27#6\n27#4\n3#1.0\n21\n35#1\n"
"63#216\n27#4\n3\n22\n28#OP_TAG\n16\n37#14\n"
"63#217\n27#2\n3#1.0\n4\n25#2\n"
"63#218\n27#3\n2#append\n22\n27#4\n48#1\n30\n35#1\n36#62\n30\n"
"63#219\n27#3\n52\n53\n26#resolve_labels\n"
"63#221\n51#optimize\n28#False\n25#1\n"
"63#221\n57#257\n"
"63#222\n3\n25#2\n"
"63#223\n3\n25#3\n"
"63#224\n28#resolve_labels\n27\n48#1\n25#4\n"
"63#225\n27#4\n52\n53\n26#optimize\n"
"63#227\n51#join_code\n"
"63#227\n57\n"
"63#228\n"
"63#229\n"
"63#230\n28#_ext_code_list\n28#_code_list\n4\n52\n53\n26#join_code\n"
"63#232\n51#_gen_code\n28#False\n25\n"
"63#232\n57#1\n"
"63#233\n"
"63#234\n"
"63#236\n28#emit\n28#OP_EOP\n48#1\n30\n"
"63#238\n28#_ext_code_list\n28#_code_list\n4\n25#1\n"
"63#240\n31\n26#_code_list\n"
"63#241\n31\n26#_ext_code_list\n"
"63#242\n28#optimize\n27#1\n48#1\n25#1\n"
"63#243\n27\n37#5\n"
"63#243\n27#1\n52\n35#1\n27#1\n45\n44#16\n25#2\n"
"63#245\n27#2\n3#1.0\n22\n24\n15\n37#7\n"
"63#246\n28#print\n27#2\n48#1\n30\n35#1\n36#15\n30\n"
"63#247\n28#save_as_bin\n27#1\n49#1\n52\n53\n26#_gen_code\n"
"63#249\n51#gen_code\n28#False\n25\n"
"63#249\n57#1\n"
"63#250\n"
"63#251\n"
"63#253\n28#emit\n28#OP_EOP\n48#1\n30\n"
"63#254\n28#_ext_code_list\n28#_code_list\n4\n25#1\n"
"63#256\n31\n26#_code_list\n"
"63#257\n31\n26#_ext_code_list\n"
"63#258\n28#optimize\n27#1\n48#1\n25#1\n"
"63#259\n27#1\n52\n53\n26#gen_code\n"
"63#262\n51#def_local\n"
"63#262\n57#256\n"
"63#263\n28#_asm_ctx\n2#add_local\n22\n27\n48#1\n30\n53\n26#def_local\n"
"63#265\n51#get_loc_num\n"
"63#265\n57\n"
"63#266\n28#len\n28#_asm_ctx\n2#scope\n22\n2#locals\n22\n49#1\n52\n53\n26#get_loc_num\n"
"63#268\n51#push_scope\n"
"63#268\n57\n"
"63#269\n28#_asm_ctx\n2#push\n22\n48\n30\n53\n26#push_scope\n"
"63#271\n51#pop_scope\n"
"63#271\n57\n"
"63#272\n28#_asm_ctx\n2#pop\n22\n48\n30\n53\n26#pop_scope\n"
"63#274\n51#emit_store\n"
"63#274\n57#256\n"
"63#275\n28#_asm_ctx\n2#store\n22\n27\n48#1\n30\n53\n26#emit_store\n"
"63#278\n51#encode_error\n"
"63#278\n57#512\n"
"63#279\n"
"63#280\n28#compile_error\n2#encode\n28#_ctx\n2#src\n22\n27\n27#1\n48#4\n30\n53\n26#encode_error\n"
"63#282\n51#load_attr\n"
"63#282\n57#256\n"
"63#283\n27\n2#type\n22\n2#name\n15\n37#7\n"
"63#284\n2#string\n27\n2#type\n21\n35#1\n"
"63#285\n28#emit_load\n27\n48#1\n30\n53\n26#load_attr\n"
"63#287\n51#store\n"
"63#287\n57#256\n"
"63#288\n27\n2#type\n22\n2#name\n15\n37#7\n"
"63#289\n28#emit_store\n27\n48#1\n30\n35#74\n27\n2#type\n22\n2#get\n15\n37#21\n"
"63#291\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#292\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#293\n28#emit\n28#OP_SET\n48#1\n30\n35#48\n27\n2#type\n22\n2#attr\n15\n37#21\n"
"63#295\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#296\n28#load_attr\n27\n2#second\n22\n48#1\n30\n"
"63#297\n28#emit\n28#OP_SET\n48#1\n30\n35#22\n27\n2#type\n22\n2#,\n15\n37#16\n"
"63#300\n28#store\n27\n2#first\n22\n48#1\n30\n"
"63#301\n28#store\n27\n2#second\n22\n48#1\n30\n35#1\n53\n26#store\n"
"63#303\n51#newglo\n"
"63#303\n57\n"
"63#304\n"
"63#305\n28#_global_index\n3#1.0\n4\n26#_global_index\n"
"63#306\n28#Token\n2#name\n2##\n28#str\n28#_global_index\n3#1.0\n5\n48#1\n4\n49#2\n52\n53\n26#newglo\n"
"63#308\n51#newtag\n"
"63#308\n57\n"
"63#309\n"
"63#310\n28#_tag_cnt\n3#1.0\n4\n26#_tag_cnt\n"
"63#311\n28#_tag_cnt\n3#1.0\n5\n52\n53\n26#newtag\n"
"63#313\n51#jump\n28#OP_JUMP\n25#1\n"
"63#313\n57#257\n"
"63#314\n28#emit\n27#1\n27\n48#2\n30\n53\n26#jump\n"
"63#316\n51#emit_tag\n"
"63#316\n57#256\n"
"63#317\n28#emit\n28#OP_TAG\n27\n48#2\n30\n53\n26#emit_tag\n"
"63#320\n51#build_set\n"
"63#320\n57#768\n"
"63#321\n28#AstNode\n2#=\n48#1\n25#3\n"
"63#322\n27#2\n27#3\n2#first\n21\n"
"63#323\n28#AstNode\n2#get\n48#1\n25#4\n"
"63#324\n27#1\n27#4\n2#first\n21\n"
"63#325\n27#2\n27#4\n2#second\n21\n"
"63#326\n27#4\n27#3\n2#first\n21\n"
"63#327\n27#3\n52\n53\n26#build_set\n"
"63#329\n51#encode_op\n"
"63#329\n57#256\n"
"63#330\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#331\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#332\n28#emit\n28#_op_dict\n27\n2#type\n22\n22\n48#1\n30\n53\n26#encode_op\n"
"63#334\n51#encode_notin\n"
"63#334\n57#256\n"
"63#335\n28#encode_in\n27\n48#1\n30\n"
"63#336\n28#emit\n28#OP_NOT\n48#1\n30\n53\n26#encode_notin\n"
"63#338\n51#encode_isnot\n"
"63#338\n57#256\n"
"63#339\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#340\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#341\n28#emit\n28#OP_NOTEQ\n48#1\n30\n53\n26#encode_isnot\n"
"63#343\n51#encode_inplace_op\n"
"63#343\n57#256\n"
"63#344\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#345\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#346\n28#emit\n28#_op_ext_dict\n27\n2#type\n22\n22\n48#1\n30\n"
"63#347\n28#store\n27\n2#first\n22\n48#1\n30\n53\n26#encode_inplace_op\n"
"63#354\n51#encode_list0\n"
"63#354\n57#256\n"
"63#355\n27\n24\n15\n37#4\n24\n52\n35#1\n"
"63#356\n31\n25#1\n"
"63#357\n27\n2#type\n22\n2#,\n15\n37#16\n"
"63#358\n27#1\n2#append\n22\n27\n2#second\n22\n48#1\n30\n"
"63#359\n27\n2#first\n22\n25\n36#20\n"
"63#360\n27#1\n2#append\n22\n27\n48#1\n30\n"
"63#361\n27#1\n2#reverse\n22\n48\n30\n27#1\n45\n44#13\n25#2\n"
"63#363\n28#encode_item\n27#2\n48#1\n30\n"
"63#364\n28#emit\n28#OP_APPEND\n48#1\n30\n36#12\n30\n53\n26#encode_list0\n"
"63#367\n51#encode_list\n"
"63#367\n57#256\n"
"63#368\n28#emit\n28#OP_LIST\n3\n48#2\n30\n"
"63#370\n27\n2#first\n22\n24\n15\n37#5\n"
"63#371\n3\n52\n35#1\n"
"63#372\n28#gettype\n27\n2#first\n22\n48#1\n2#list\n15\n37#20\n27\n2#first\n22\n45\n44#13\n25#1\n"
"63#374\n28#encode_item\n27#1\n48#1\n30\n"
"63#375\n28#emit\n28#OP_APPEND\n48#1\n30\n36#12\n30\n35#13\n"
"63#377\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#378\n28#emit\n28#OP_APPEND\n48#1\n30\n"
"63#379\n3#1.0\n52\n53\n26#encode_list\n"
"63#381\n51#encode_tuple\n"
"63#381\n57#256\n"
"63#383\n28#gettype\n27\n2#first\n22\n48#1\n2#list\n15\n38#7\n28#is_const_list\n27\n2#first\n22\n48#1\n19\n37#17\n"
"63#384\n28#get_const_list\n27\n2#first\n22\n48#1\n25#1\n"
"63#385\n28#emit_load\n27#1\n48#1\n30\n"
"63#386\n3#1.0\n52\n35#1\n"
"63#387\n28#encode_list\n27\n49#1\n52\n53\n26#encode_tuple\n"
"63#389\n51#encode_comma\n"
"63#389\n57#256\n"
"63#390\n28#encode_item\n27\n2#first\n22\n48#1\n28#encode_item\n27\n2#second\n22\n48#1\n4\n52\n53\n26#encode_comma\n"
"63#392\n51#is_const_list\n"
"63#392\n57#256\n27\n45\n44#26\n25#1\n"
"63#394\n28#hasattr\n27#1\n2#type\n48#2\n10\n37#5\n"
"63#395\n28#False\n52\n35#1\n"
"63#396\n27#1\n2#type\n22\n28##0\n17\n10\n37#5\n"
"63#397\n28#False\n52\n35#1\n36#25\n30\n"
"63#398\n28#True\n52\n53\n26#is_const_list\n"
"63#400\n51#get_const_list\n"
"63#400\n57#256\n"
"63#401\n28#asm_switch__code_list\n48\n30\n"
"63#402\n28#newglo\n48\n25#1\n"
"63#403\n28#emit\n28#OP_LIST\n48#1\n30\n27\n45\n44#13\n25#2\n"
"63#405\n28#encode_item\n27#2\n48#1\n30\n"
"63#406\n28#emit\n28#OP_APPEND\n48#1\n30\n36#12\n30\n"
"63#407\n28#store_global\n27#1\n48#1\n30\n"
"63#408\n28#asm_switch__code_list\n48\n30\n"
"63#409\n27#1\n52\n53\n26#get_const_list\n"
"63#412\n51#encode_if\n"
"63#412\n57#256\n"
"63#414\n27\n2#first\n22\n2#type\n22\n2#=\n15\n37#10\n"
"63#415\n28#encode_error\n27\n2#first\n22\n2#do not allow assignment in if condition\n48#2\n30\n35#1\n"
"63#416\n28#encode_item\n27\n2#first\n22\n48#1\n30\n28#newtag\n48\n28#newtag\n48\n41#2\n25#1\n25#2\n"
"63#418\n28#jump\n27#1\n28#OP_POP_JUMP_ON_FALSE\n48#2\n30\n"
"63#420\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#421\n28#jump\n27#2\n48#1\n30\n"
"63#422\n28#emit_tag\n27#1\n48#1\n30\n"
"63#423\n28#encode_item\n27\n2#third\n22\n48#1\n30\n"
"63#424\n28#emit_tag\n27#2\n48#1\n30\n53\n26#encode_if\n"
"63#426\n51#encode_assign_to\n"
"63#426\n57#512\n"
"63#427\n28#istype\n27\n2#list\n48#2\n37#41\n"
"63#428\n28#len\n27\n48#1\n3#1.0\n15\n37#3\n"
"63#430\n35#20\n27#1\n3#1.0\n15\n37#10\n"
"63#432\n28#emit\n28#OP_UNPACK\n28#len\n27\n48#1\n48#2\n30\n35#7\n"
"63#434\n28#emit\n28#OP_ROT\n27#1\n48#2\n30\n27\n45\n44#8\n25#2\n"
"63#436\n28#store\n27#2\n48#1\n30\n36#7\n30\n35#6\n"
"63#438\n28#store\n27\n48#1\n30\n53\n26#encode_assign_to\n"
"63#440\n51#encode_assign\n"
"63#440\n57#256\n"
"63#441\n28#gettype\n27\n2#second\n22\n48#1\n2#list\n15\n37#22\n27\n2#second\n22\n45\n44#8\n25#1\n"
"63#443\n28#encode_item\n27#1\n48#1\n30\n36#7\n30\n"
"63#444\n28#len\n27\n2#second\n22\n48#1\n25#2\n35#11\n"
"63#446\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#447\n3#1.0\n25#2\n"
"63#448\n28#encode_assign_to\n27\n2#first\n22\n27#2\n48#2\n30\n53\n26#encode_assign\n"
"63#450\n51#encode_dict\n"
"63#450\n57#256\n"
"63#451\n27\n2#first\n22\n25#1\n"
"63#452\n28#emit\n28#OP_DICT\n3\n48#2\n30\n"
"63#453\n27#1\n24\n16\n37#27\n27#1\n45\n44#22\n25#2\n"
"63#455\n28#encode_item\n27#2\n3\n22\n48#1\n30\n"
"63#456\n28#encode_item\n27#2\n3#1.0\n22\n48#1\n30\n"
"63#457\n28#emit\n28#OP_DICT_SET\n48#1\n30\n36#21\n30\n35#1\n53\n26#encode_dict\n"
"63#459\n51#encode_neg\n"
"63#459\n57#256\n"
"63#460\n27\n2#first\n22\n2#type\n22\n2#number\n15\n37#20\n"
"63#461\n27\n2#first\n22\n25\n"
"63#462\n27\n2#val\n22\n9\n27\n2#val\n21\n"
"63#463\n28#encode_item\n27\n48#1\n30\n35#13\n"
"63#465\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#466\n28#emit\n28#OP_NEG\n48#1\n30\n53\n26#encode_neg\n"
"63#468\n51#encode_not\n"
"63#468\n57#256\n"
"63#469\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#470\n28#emit\n28#OP_NOT\n48#1\n30\n53\n26#encode_not\n"
"63#473\n51#encode_call\n"
"63#473\n57#256\n"
"63#474\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#475\n28#gettype\n27\n2#second\n22\n48#1\n2#list\n15\n37#22\n27\n2#second\n22\n45\n44#8\n25#1\n"
"63#477\n28#encode_item\n27#1\n48#1\n30\n36#7\n30\n"
"63#478\n28#len\n27\n2#second\n22\n48#1\n25#2\n35#8\n"
"63#480\n28#encode_item\n27\n2#second\n22\n48#1\n25#2\n"
"63#481\n28#emit\n28#OP_CALL\n27#2\n48#2\n30\n53\n26#encode_call\n"
"63#483\n51#encode_apply\n"
"63#483\n57#256\n"
"63#484\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#485\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#486\n28#emit\n28#OP_APPLY\n48#1\n30\n53\n26#encode_apply\n"
"63#489\n51#encode_def\n3\n25#1\n"
"63#489\n57#257\n"
"63#490\n28#emit_def\n27\n2#first\n22\n48#1\n30\n"
"63#491\n28#push_scope\n48\n30\n"
"63#492\n3\n25#2\n"
"63#493\n3\n25#3\n"
"63#494\n3\n25#4\n"
"63#495\n3\n25#5\n27\n2#second\n22\n45\n44#58\n25#6\n"
"63#497\n28#def_local\n27#6\n2#first\n22\n48#1\n30\n"
"63#499\n27#6\n2#type\n22\n2#narg\n15\n37#7\n"
"63#500\n3#1.0\n25#2\n"
"63#502\n35#38\n35#1\n"
"63#503\n27#6\n2#second\n22\n37#21\n"
"63#504\n27#4\n3#1.0\n4\n25#4\n"
"63#505\n28#encode_item\n27#6\n2#second\n22\n48#1\n30\n"
"63#506\n28#store\n27#6\n2#first\n22\n48#1\n30\n35#6\n"
"63#508\n27#3\n3#1.0\n4\n25#3\n"
"63#509\n27#5\n3#1.0\n4\n25#5\n36#57\n30\n"
"63#510\n28#getlineno\n27\n2#first\n22\n48#1\n25#7\n"
"63#511\n27#7\n24\n16\n37#8\n"
"63#512\n28#emit\n28#OP_LINE\n27#7\n48#2\n30\n35#1\n"
"63#513\n27#2\n10\n37#12\n"
"63#514\n28#emit\n28#OP_LOAD_PARAMS\n27#3\n3#256.0\n6\n27#4\n4\n48#2\n30\n35#12\n27#3\n3\n11\n37#8\n"
"63#516\n28#emit\n28#OP_LOAD_PARG\n27#3\n48#2\n30\n35#1\n"
"63#517\n27#2\n37#8\n"
"63#518\n28#emit\n28#OP_LOAD_NARG\n27#5\n48#2\n30\n35#1\n"
"63#519\n28#encode_item\n27\n2#third\n22\n48#1\n30\n"
"63#520\n28#emit\n28#OP_DEF_END\n48#1\n30\n"
"63#524\n28#pop_scope\n48\n30\n"
"63#525\n27#1\n10\n37#9\n"
"63#526\n28#emit_store\n27\n2#first\n22\n48#1\n30\n35#1\n53\n26#encode_def\n"
"63#529\n51#encode_class\n"
"63#529\n57#256\n"
"63#530\n31\n25#1\n"
"63#531\n28#emit\n28#OP_CLASS\n27\n2#first\n22\n2#val\n22\n48#2\n30\n27\n2#second\n22\n45\n44#52\n25#2\n"
"63#533\n27#2\n2#type\n22\n2#pass\n15\n37#4\n"
"63#533\n36#10\n35#1\n"
"63#534\n27#2\n2#type\n22\n2#def\n16\n37#8\n"
"63#535\n28#encode_error\n27#2\n2#non-func expression in class is invalid\n48#2\n30\n35#1\n"
"63#536\n28#encode_def\n27#2\n3#1.0\n48#2\n30\n"
"63#537\n28#emit_load\n27\n2#first\n22\n48#1\n30\n"
"63#538\n28#load_attr\n27#2\n2#first\n22\n48#1\n30\n"
"63#539\n28#emit\n28#OP_SET\n48#1\n30\n36#51\n30\n"
"63#540\n28#emit\n28#OP_CLASS_END\n48#1\n30\n53\n26#encode_class\n"
"63#542\n51#encode_return\n"
"63#542\n57#256\n"
"63#543\n27\n2#first\n22\n37#63\n"
"63#544\n28#gettype\n27\n2#first\n22\n48#1\n2#list\n15\n37#25\n"
"63#546\n28#emit\n28#OP_LIST\n48#1\n30\n27\n2#first\n22\n45\n44#13\n25#1\n"
"63#548\n28#encode_item\n27#1\n48#1\n30\n"
"63#549\n28#emit\n28#OP_APPEND\n48#1\n30\n36#12\n30\n35#29\n"
"63#551\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#552\n27\n2#first\n22\n2#type\n22\n2#call\n15\n37#13\n28#code_pop\n48\n40#2\n25#2\n25#3\n"
"63#554\n28#emit\n28#OP_TAILCALL\n27#3\n48#2\n30\n35#1\n35#6\n"
"63#557\n28#emit_load\n24\n48#1\n30\n"
"63#558\n28#emit\n28#OP_RETURN\n48#1\n30\n53\n26#encode_return\n"
"63#560\n51#encode_while\n"
"63#560\n57#256\n28#newtag\n48\n28#newtag\n48\n41#2\n25#1\n25#2\n"
"63#563\n28#_begin_tag_list\n2#append\n22\n27#1\n48#1\n30\n"
"63#564\n28#_end_tag_list\n2#append\n22\n27#2\n48#1\n30\n"
"63#566\n28#emit_tag\n27#1\n48#1\n30\n"
"63#567\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#568\n28#jump\n27#2\n28#OP_POP_JUMP_ON_FALSE\n48#2\n30\n"
"63#569\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#571\n28#jump\n27#1\n48#1\n30\n"
"63#572\n28#emit_tag\n27#2\n48#1\n30\n"
"63#574\n28#_begin_tag_list\n2#pop\n22\n48\n30\n"
"63#575\n28#_end_tag_list\n2#pop\n22\n48\n30\n53\n26#encode_while\n"
"63#577\n51#encode_continue\n"
"63#577\n57#256\n"
"63#579\n28#jump\n28#_begin_tag_list\n3#-1.0\n22\n48#1\n30\n53\n26#encode_continue\n"
"63#582\n51#encode_break\n"
"63#582\n57#256\n"
"63#583\n28#jump\n28#_end_tag_list\n3#-1.0\n22\n48#1\n30\n53\n26#encode_break\n"
"63#585\n51#encode_import_one\n"
"63#585\n57#512\n"
"63#587\n28#encode_item\n27\n48#1\n30\n"
"63#588\n2#string\n27#1\n2#type\n21\n"
"63#589\n28#encode_item\n27#1\n48#1\n30\n"
"63#590\n28#emit\n28#OP_IMPORT\n3#2.0\n48#2\n30\n53\n26#encode_import_one\n"
"63#592\n51#_import_name2str\n"
"63#592\n57#256\n"
"63#593\n27\n2#type\n22\n2#attr\n15\n37#30\n"
"63#594\n28#_import_name2str\n27\n2#first\n22\n48#1\n25#1\n"
"63#595\n28#_import_name2str\n27\n2#second\n22\n48#1\n25#2\n"
"63#596\n28#Token\n2#string\n27#1\n2#val\n22\n2#/\n4\n27#2\n2#val\n22\n4\n49#2\n52\n35#26\n27\n2#type\n22\n2#name\n15\n37#10\n"
"63#598\n2#string\n27\n2#type\n21\n"
"63#599\n27\n52\n35#11\n27\n2#type\n22\n2#string\n15\n37#5\n"
"63#601\n27\n52\n35#1\n53\n26#_import_name2str\n"
"63#603\n51#encode_import_multi\n"
"63#603\n57#512\n"
"63#604\n28#_import_name2str\n27\n48#1\n25\n"
"63#605\n27#1\n2#type\n22\n2#,\n15\n37#18\n"
"63#606\n28#encode_import_multi\n27\n27#1\n2#first\n22\n48#2\n30\n"
"63#607\n28#encode_import_multi\n27\n27#1\n2#second\n22\n48#2\n30\n35#7\n"
"63#609\n28#encode_import_one\n27\n27#1\n48#2\n30\n53\n26#encode_import_multi\n"
"63#611\n51#encode_from\n"
"63#611\n57#256\n"
"63#612\n28#encode_import_multi\n27\n2#first\n22\n27\n2#second\n22\n48#2\n30\n53\n26#encode_from\n"
"63#614\n51#_encode_import\n"
"63#614\n57#256\n"
"63#615\n2#string\n27\n2#type\n21\n"
"63#616\n28#encode_item\n27\n48#1\n30\n"
"63#617\n28#emit\n28#OP_IMPORT\n3#1.0\n48#2\n30\n53\n26#_encode_import\n"
"63#619\n51#encode_import\n"
"63#619\n57#256\n"
"63#620\n27\n2#first\n22\n2#type\n22\n2#,\n15\n37#21\n"
"63#621\n27\n2#first\n22\n25\n"
"63#622\n28#_encode_import\n27\n2#first\n22\n48#1\n30\n"
"63#623\n28#_encode_import\n27\n2#second\n22\n48#1\n30\n35#8\n"
"63#625\n28#_encode_import\n27\n2#first\n22\n48#1\n30\n53\n26#encode_import\n"
"63#628\n51#encode_and\n"
"63#628\n57#256\n"
"63#629\n28#newtag\n48\n25#1\n"
"63#630\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#631\n28#emit\n28#OP_JUMP_ON_FALSE\n27#1\n48#2\n30\n"
"63#632\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#633\n28#emit\n28#OP_AND\n48#1\n30\n"
"63#634\n28#emit_tag\n27#1\n48#1\n30\n53\n26#encode_and\n"
"63#637\n51#encode_or\n"
"63#637\n57#256\n"
"63#638\n28#newtag\n48\n25#1\n"
"63#639\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#640\n28#emit\n28#OP_JUMP_ON_TRUE\n27#1\n48#2\n30\n"
"63#641\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#642\n28#emit\n28#OP_OR\n48#1\n30\n"
"63#643\n28#emit_tag\n27#1\n48#1\n30\n53\n26#encode_or\n"
"63#645\n51#encode_raw_list\n"
"63#645\n57#256\n"
"63#646\n28#gettype\n27\n48#1\n2#list\n15\n37#28\n"
"63#647\n28#emit\n28#OP_LIST\n48#1\n30\n27\n45\n44#13\n25#1\n"
"63#649\n28#encode_item\n27#1\n48#1\n30\n"
"63#650\n28#emit\n28#OP_APPEND\n48#1\n30\n36#12\n30\n"
"63#651\n28#len\n27\n49#1\n52\n35#9\n"
"63#653\n28#encode_item\n27\n48#1\n30\n"
"63#654\n3#1.0\n52\n53\n26#encode_raw_list\n"
"63#656\n51#encode_for\n"
"63#656\n57#256\n"
"63#657\n28#newtag\n48\n25#1\n"
"63#658\n28#newtag\n48\n25#2\n"
"63#660\n28#_begin_tag_list\n2#append\n22\n27#1\n48#1\n30\n"
"63#661\n28#_end_tag_list\n2#append\n22\n27#2\n48#1\n30\n"
"63#663\n27\n2#first\n22\n2#second\n22\n25#3\n"
"63#664\n27\n2#first\n22\n2#first\n22\n25#4\n"
"63#667\n28#encode_raw_list\n27#3\n48#1\n30\n"
"63#668\n28#emit\n28#OP_ITER\n48#1\n30\n"
"63#669\n28#emit_tag\n27#1\n48#1\n30\n"
"63#670\n28#jump\n27#2\n28#OP_NEXT\n48#2\n30\n"
"63#671\n28#encode_assign_to\n27#4\n3#1.0\n48#2\n30\n"
"63#674\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#675\n28#jump\n27#1\n48#1\n30\n"
"63#676\n28#emit_tag\n27#2\n48#1\n30\n"
"63#678\n28#_begin_tag_list\n2#pop\n22\n48\n30\n"
"63#679\n28#_end_tag_list\n2#pop\n22\n48\n30\n"
"63#680\n28#emit\n28#OP_POP\n48#1\n30\n53\n26#encode_for\n"
"63#682\n51#encode_global\n"
"63#682\n57#256\n"
"63#683\n28#add_global\n27\n2#first\n22\n48#1\n30\n53\n26#encode_global\n"
"63#685\n51#encode_try\n"
"63#685\n57#256\n"
"63#686\n28#newtag\n48\n25#1\n"
"63#687\n28#newtag\n48\n25#2\n"
"63#688\n28#chk_try_block\n27#1\n48#1\n10\n37#8\n"
"63#689\n28#encode_error\n27\n2#do not support recursive try\n48#2\n30\n35#1\n"
"63#690\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#691\n28#emit\n28#OP_CLR_JUMP\n48#1\n30\n"
"63#692\n28#jump\n27#2\n48#1\n30\n"
"63#693\n28#emit_tag\n27#1\n48#1\n30\n"
"63#694\n27\n2#second\n22\n24\n16\n37#14\n"
"63#695\n28#emit\n28#OP_LOAD_EX\n48#1\n30\n"
"63#696\n28#store\n27\n2#second\n22\n48#1\n30\n35#6\n"
"63#698\n28#emit\n28#OP_POP\n48#1\n30\n"
"63#699\n28#encode_item\n27\n2#third\n22\n48#1\n30\n"
"63#700\n28#emit_tag\n27#2\n48#1\n30\n"
"63#701\n28#exit_try_block\n48\n30\n53\n26#encode_try\n"
"63#703\n51#do_nothing\n"
"63#703\n57#256\n"
"63#704\n53\n26#do_nothing\n"
"63#706\n51#encode_del\n"
"63#706\n57#256\n"
"63#707\n27\n2#first\n22\n25#1\n"
"63#708\n27#1\n2#type\n22\n2#get\n16\n38#7\n27#1\n2#type\n22\n2#attr\n16\n19\n37#8\n"
"63#709\n28#encode_error\n27#1\n2#require get or attr expression\n48#2\n30\n35#1\n"
"63#710\n28#encode_item\n27#1\n2#first\n22\n48#1\n30\n"
"63#711\n27#1\n2#type\n22\n2#attr\n15\n37#9\n"
"63#712\n28#load_attr\n27#1\n2#second\n22\n48#1\n30\n35#8\n"
"63#714\n28#encode_item\n27#1\n2#second\n22\n48#1\n30\n"
"63#715\n28#emit\n28#OP_DEL\n48#1\n30\n53\n26#encode_del\n"
"63#717\n51#encode_annotation\n"
"63#717\n57#256\n"
"63#718\n27\n2#first\n22\n25#1\n"
"63#719\n27#1\n2#val\n22\n2#debugger\n15\n37#7\n"
"63#720\n28#emit\n28#OP_DEBUG\n48#1\n30\n35#1\n53\n26#encode_annotation\n"
"63#722\n51#encode_attr\n"
"63#722\n57#256\n"
"63#723\n2#string\n27\n2#second\n22\n2#type\n21\n"
"63#724\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#725\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#726\n28#emit\n28#OP_GET\n48#1\n30\n53\n26#encode_attr\n"
"63#728\n51#encode_slice\n"
"63#728\n57#256\n"
"63#729\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#730\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#731\n28#encode_item\n27\n2#third\n22\n48#1\n30\n"
"63#732\n28#emit\n28#OP_SLICE\n48#1\n30\n53\n26#encode_slice\n"
"63#734\n51#encode_in\n"
"63#734\n57#256\n"
"63#735\n28#encode_item\n27\n2#first\n22\n48#1\n30\n"
"63#736\n28#encode_item\n27\n2#second\n22\n48#1\n30\n"
"63#737\n28#emit\n28#OP_IN\n48#1\n30\n53\n26#encode_in\n"
"63#740\n33\n2#if\n28#encode_if\n34\n2#=\n28#encode_assign\n34\n2#tuple\n28#encode_tuple\n34\n2#,\n28#encode_comma\n34\n2#dict\n28#encode_dict\n34\n2#call\n28#encode_call\n34\n2#apply\n28#encode_apply\n34\n2#neg\n28#encode_neg\n34\n2#not\n28#encode_not\n34\n2#list\n28#encode_list\n34\n2#def\n28#encode_def\n34\n2#del\n28#encode_del\n34\n2#class\n28#encode_class\n34\n2#return\n28#encode_return\n34\n2#while\n28#encode_while\n34\n2#continue\n28#encode_continue\n34\n2#break\n28#encode_break\n34\n2#from\n28#encode_from\n34\n2#import\n28#encode_import\n34\n2#and\n28#encode_and\n34\n2#or\n28#encode_or\n34\n2#for\n28#encode_for\n34\n2#global\n28#encode_global\n34\n2#name\n28#emit_load\n34\n2#number\n28#emit_load\n34\n2#string\n28#emit_load\n34\n2#None\n28#emit_load\n34\n2#True\n28#emit_load\n34\n2#False\n28#emit_load\n34\n2#try\n28#encode_try\n34\n2#pass\n28#do_nothing\n34\n2#notin\n28#encode_notin\n34\n2#isnot\n28#encode_isnot\n34\n2#attr\n28#encode_attr\n34\n2#slice\n28#encode_slice\n34\n2#in\n28#encode_in\n34\n2#@\n28#encode_annotation\n34\n26#_encode_dict\n28#_op_dict\n45\n44#8\n26#k\n"
"63#781\n28#encode_op\n28#_encode_dict\n28#k\n21\n36#7\n30\n28#_op_ext_dict\n45\n44#8\n26#k\n"
"63#783\n28#encode_inplace_op\n28#_encode_dict\n28#k\n21\n36#7\n30\n"
"63#785\n51#getlineno\n"
"63#785\n57#256\n"
"63#786\n28#hasattr\n27\n2#pos\n48#2\n37#9\n"
"63#787\n27\n2#pos\n22\n3\n22\n52\n35#14\n28#hasattr\n27\n2#first\n48#2\n37#9\n"
"63#789\n28#getlineno\n27\n2#first\n22\n49#1\n52\n35#1\n"
"63#790\n24\n52\n53\n26#getlineno\n"
"63#792\n51#encode_item\n"
"63#792\n57#256\n"
"63#797\n27\n24\n15\n37#5\n"
"63#798\n3\n52\n35#1\n"
"63#800\n28#gettype\n27\n48#1\n2#list\n15\n37#7\n"
"63#801\n28#encode_block\n27\n49#1\n52\n35#1\n"
"63#803\n28#_encode_dict\n27\n2#type\n22\n22\n27\n48#1\n25#1\n"
"63#804\n27#1\n24\n16\n37#5\n"
"63#805\n27#1\n52\n35#1\n"
"63#806\n3#1.0\n52\n53\n26#encode_item\n"
"63#808\n51#need_pop_value\n"
"63#808\n57#256\n"
"63#809\n27\n28#POP_VALUE_TYPE_SET\n17\n52\n53\n26#need_pop_value\n"
"63#811\n51#encode_block\n"
"63#811\n57#256\n"
"63#812\n28#assert\n28#gettype\n27\n48#1\n2#list\n15\n48#1\n30\n27\n45\n44#48\n25#1\n"
"63#815\n27#1\n2#type\n22\n2#string\n15\n37#4\n"
"63#816\n36#10\n35#1\n"
"63#817\n28#getlineno\n27#1\n48#1\n25#2\n"
"63#818\n27#2\n24\n16\n37#8\n"
"63#819\n28#emit\n28#OP_LINE\n27#2\n48#2\n30\n35#1\n"
"63#820\n28#encode_item\n27#1\n48#1\n30\n"
"63#821\n28#need_pop_value\n27#1\n2#type\n22\n48#1\n37#7\n"
"63#822\n28#emit\n28#OP_POP\n48#1\n30\n35#1\n36#47\n30\n53\n26#encode_block\n"
"63#824\n51#encode\n"
"63#824\n57#256\n"
"63#825\n"
"63#826\n"
"63#827\n"
"63#828\n3\n26#_tag_cnt\n"
"63#829\n3\n25#1\n"
"63#830\n28#parse\n27\n48#1\n25#2\n"
"63#831\n28#encode_item\n27#2\n48#1\n30\n53\n26#encode\n"
"63#833\n54#EncodeCtx\n51#__init__\n"
"63#837\n57#512\n"
"63#838\n27#1\n27\n2#src\n21\n"
"63#839\n31\n27\n2#code_list\n21\n53\n28#EncodeCtx\n2#__init__\n21\n51#set_file_name\n"
"63#841\n57#512\n"
"63#842\n27#1\n2#split\n22\n2#.\n48#1\n3\n22\n25#2\n"
"63#843\n28#emit\n28#OP_FILE\n27#2\n48#2\n30\n53\n28#EncodeCtx\n2#set_file_name\n21\n51#compile\n"
"63#845\n57#256\n"
"63#846\n28#encode\n27\n2#src\n22\n48#1\n27\n2#ast\n21\n"
"63#847\n27\n2#ast\n22\n52\n53\n28#EncodeCtx\n2#compile\n21\n51#gen_code\n"
"63#849\n57#256\n"
"63#850\n28#join_code\n48\n25#1\n"
"63#851\n27#1\n52\n53\n28#EncodeCtx\n2#gen_code\n21\n56\n"
"63#853\n51#_compile\n24\n25#2\n"
"63#853\n57#513\n"
"63#854\n"
"63#856\n28#asm_init\n48\n30\n"
"63#857\n28#EncodeCtx\n27\n48#1\n26#_ctx\n"
"63#858\n28#_ctx\n2#set_file_name\n22\n28#name\n48#1\n30\n"
"63#859\n28#_ctx\n2#compie\n22\n48\n30\n"
"63#860\n28#_ctx\n2#gen_code\n22\n49\n52\n53\n26#_compile\n"
"63#862\n51#escape_for_compile\n"
"63#862\n57#256\n"
"63#863\n28#str\n27\n48#1\n25\n"
"63#864\n27\n2#replace\n22\n2#\\\\\n2#\\\\\\\\\n48#2\n25\n"
"63#865\n27\n2#replace\n22\n2#\\r\n2#\\\\r\n48#2\n25\n"
"63#866\n27\n2#replace\n22\n2#\\n\n2#\\\\n\n48#2\n25\n"
"63#867\n27\n2#replace\n22\n2#\\0\n2#\\\\0\n48#2\n25\n"
"63#868\n27\n52\n53\n26#escape_for_compile\n"
"63#870\n51#compile_to_list\n"
"63#870\n57#512\n"
"63#871\n"
"63#873\n28#asm_init\n48\n30\n"
"63#874\n28#EncodeCtx\n27\n48#1\n26#_ctx\n"
"63#876\n28#emit\n28#OP_FILE\n27#1\n48#2\n30\n"
"63#877\n28#encode\n27\n48#1\n30\n"
"63#878\n24\n26#_ctx\n"
"63#879\n28#gen_code\n48\n25#2\n"
"63#880\n27#2\n52\n53\n26#compile_to_list\n"
"63#882\n51#compile\n24\n25#2\n"
"63#882\n57#513\n"
"63#883\n28#Compiler\n48\n25#3\n"
"63#884\n27#3\n2#compile\n22\n27\n27#1\n27#2\n49#3\n52\n53\n26#compile\n"
"63#887\n51#_compilefile\n24\n25#1\n"
"63#887\n57#257\n"
"63#888\n28#_compile\n28#load\n27\n48#1\n27\n27#1\n49#3\n52\n53\n26#_compilefile\n"
"63#890\n51#compilefile\n24\n25#1\n"
"63#890\n57#257\n"
"63#891\n28#Compiler\n48\n25#2\n"
"63#892\n27#2\n2#compilefile\n22\n27\n27#1\n49#2\n52\n53\n26#compilefile\n"
"63#894\n51#split_instr\n"
"63#894\n57#256\n"
"63#895\n28#len\n27\n48#1\n25#1\n"
"63#896\n31\n25#2\n"
"63#897\n3\n25#3\n"
"63#898\n27#3\n27#1\n12\n37#39\n"
"63#899\n27\n27#3\n22\n25#4\n"
"63#900\n28#uncode16\n27\n27#3\n3#1.0\n4\n22\n27\n27#3\n3#2.0\n4\n22\n48#2\n25#5\n"
"63#901\n27#3\n3#3.0\n4\n25#3\n"
"63#902\n27#2\n2#append\n22\n31\n28#ord\n27#4\n48#1\n32\n27#5\n32\n48#1\n30\n36#41\n"
"63#903\n27#2\n52\n53\n26#split_instr\n"
"63#905\n51#to_fixed\n"
"63#905\n57#512\n"
"63#906\n28#str\n27\n48#1\n2#rjust\n22\n27#1\n48#1\n2#replace\n22\n2# \n2#0\n49#2\n52\n53\n26#to_fixed\n"
"63#908\n51#dis_code\n28#False\n25#1\n2#<string>\n25#2\n"
"63#908\n57#258\n"
"63#909\n27#1\n28#True\n15\n37#5\n"
"63#910\n31\n25#3\n35#1\n"
"63#912\n28#compile_to_list\n27\n27#2\n48#2\n25#4\n28#enumerate\n27#4\n48#1\n45\n44#52\n40#2\n25#5\n25#6\n"
"63#914\n28#int\n27#6\n3\n22\n48#1\n25#7\n"
"63#915\n2#%s %s %r\n31\n28#to_fixed\n27#5\n3#1.0\n4\n3#4.0\n48#2\n32\n28#opcodes\n27#7\n22\n2#ljust\n22\n3#22.0\n48#1\n32\n27#6\n3#1.0\n22\n32\n8\n25#8\n"
"63#918\n27#1\n37#9\n"
"63#919\n27#3\n2#append\n22\n27#8\n48#1\n30\n35#6\n"
"63#921\n28#print\n27#8\n48#1\n30\n36#51\n30\n"
"63#923\n27#1\n37#9\n"
"63#924\n2#\\n\n2#join\n22\n27#3\n49#1\n52\n35#1\n53\n26#dis_code\n"
"63#926\n51#dis\n28#False\n25#1\n"
"63#926\n57#257\n"
"63#927\n28#load\n27\n48#1\n25#2\n"
"63#928\n28#dis_code\n27#2\n27#1\n27\n49#3\n52\n53\n26#dis\n"
"63#931\n54#Compiler\n51#__init__\n"
"63#933\n57#256\n"
"63#934\n2#line\n27\n2#code_style\n21\n53\n28#Compiler\n2#__init__\n21\n51#set_code_style\n"
"63#936\n57#512\n"
"63#937\n27#1\n27\n2#code_style\n21\n53\n28#Compiler\n2#set_code_style\n21\n51#_escape\n"
"63#939\n57#512\n"
"63#940\n28#str\n27#1\n48#1\n25#1\n"
"63#941\n27#1\n2#replace\n22\n2#\\\\\n2#\\\\\\\\\n48#2\n25#1\n"
"63#942\n27#1\n2#replace\n22\n2#\"\n2#\\\\\"\n48#2\n25#1\n"
"63#943\n27#1\n2#replace\n22\n2#\\r\n2#\\\\r\n48#2\n25#1\n"
"63#944\n27#1\n2#replace\n22\n2#\\n\n2#\\\\n\n48#2\n25#1\n"
"63#945\n27#1\n2#replace\n22\n2#\\0\n2#\\\\0\n48#2\n25#1\n"
"63#946\n27#1\n52\n53\n28#Compiler\n2#_escape\n21\n51#gen_code\n"
"63#948\n57#768\n"
"63#949\n"
"63#951\n28#asm_init\n48\n30\n"
"63#952\n28#EncodeCtx\n27#1\n48#1\n26#_ctx\n"
"63#953\n27#2\n2#split\n22\n2#.\n48#1\n3\n22\n25#3\n"
"63#954\n28#emit\n28#OP_FILE\n27#3\n48#2\n30\n"
"63#955\n28#encode\n27#1\n48#1\n30\n"
"63#956\n24\n26#_ctx\n"
"63#957\n28#gen_code\n49\n52\n53\n28#Compiler\n2#gen_code\n21\n51#compile\n24\n25#3\n"
"63#959\n57#769\n"
"63#960\n27\n2#gen_code\n22\n27#1\n27#2\n48#2\n25#4\n"
"63#961\n2#\n25#5\n27#4\n45\n44#41\n25#6\n"
"63#964\n27#6\n3#1.0\n22\n3\n15\n37#13\n"
"63#965\n27#5\n28#str\n27#6\n3\n22\n48#1\n2#\\n\n4\n4\n25#5\n35#20\n"
"63#967\n27#5\n28#str\n27#6\n3\n22\n48#1\n2##\n4\n28#escape_for_compile\n27#6\n3#1.0\n22\n48#1\n4\n2#\\n\n4\n4\n25#5\n36#40\n30\n"
"63#968\n27#5\n52\n53\n28#Compiler\n2#compile\n21\n51#compilefile\n24\n25#2\n"
"63#970\n57#513\n"
"63#971\n27\n2#compile\n22\n28#load\n27#1\n48#1\n27#1\n27#2\n49#3\n52\n53\n28#Compiler\n2#compilefile\n21\n51#to_c_code\n24\n25#3\n"
"63#973\n57#769\n"
"63#974\n27\n2#_escape\n22\n27#2\n48#1\n25#2\n"
"63#976\n27#3\n24\n15\n37#13\n"
"63#977\n27#1\n2#split\n22\n2#.\n48#1\n3\n22\n2#_bin\n4\n25#3\n35#1\n"
"63#979\n2#const char* \n27#3\n4\n2#=\n4\n25#4\n"
"63#980\n27#4\n2#\"\n4\n25#4\n"
"63#981\n27#4\n27#2\n4\n25#4\n"
"63#982\n27#4\n2#\";\n4\n25#4\n"
"63#983\n27#4\n52\n53\n28#Compiler\n2#to_c_code\n21\n51#compile_to_c_code\n24\n25#2\n"
"63#985\n57#513\n"
"63#986\n28#load\n27#1\n48#1\n25#3\n"
"63#987\n27\n2#gen_code\n22\n27#3\n27#1\n48#2\n25#4\n"
"63#988\n31\n25#5\n"
"63#989\n31\n25#6\n27#4\n45\n44#78\n25#7\n"
"63#992\n27#7\n3\n22\n28#OP_LINE\n15\n37#28\n"
"63#993\n2#\n2#join\n22\n27#6\n48#1\n25#8\n"
"63#994\n2#\"%s\"\n27\n2#_escape\n22\n27#8\n48#1\n8\n25#8\n"
"63#995\n27#5\n2#append\n22\n27#8\n48#1\n30\n"
"63#996\n31\n25#6\n35#1\n"
"63#999\n27#7\n3#1.0\n22\n3\n15\n37#11\n"
"63#1000\n28#str\n27#7\n3\n22\n48#1\n2#\\n\n4\n25#9\n35#18\n"
"63#1002\n28#str\n27#7\n3\n22\n48#1\n2##\n4\n28#escape_for_compile\n27#7\n3#1.0\n22\n48#1\n4\n2#\\n\n4\n25#9\n"
"63#1004\n27#6\n2#append\n22\n27#9\n48#1\n30\n36#77\n30\n"
"63#1007\n28#len\n27#6\n48#1\n3\n11\n37#25\n"
"63#1008\n2#\n2#join\n22\n27#6\n48#1\n25#8\n"
"63#1009\n2#\"%s\"\n27\n2#_escape\n22\n27#8\n48#1\n8\n25#8\n"
"63#1010\n27#5\n2#append\n22\n27#8\n48#1\n30\n35#1\n"
"63#1012\n2#\\n\n2#join\n22\n27#5\n48#1\n25#4\n"
"63#1014\n27#2\n24\n15\n37#13\n"
"63#1015\n27#1\n2#split\n22\n2#.\n48#1\n3\n22\n2#_bin\n4\n25#2\n35#1\n"
"63#1017\n2#const char* \n27#2\n4\n2#=\n4\n25#10\n"
"63#1018\n27#10\n27#4\n2#;\n4\n4\n25#10\n"
"63#1019\n27#10\n52\n53\n28#Compiler\n2#compile_to_c_code\n21\n56\n"
"63#1022\n51#main\n"
"63#1022\n57\n"
"63#1023\n2#sys\n1#1\n"
"63#1024\n28#sys\n2#argv\n22\n25\n"
"63#1026\n28#len\n27\n48#1\n3#2.0\n12\n37#20\n"
"63#1027\n28#print\n2#usage: %s filename    : compile python to c code\n27\n3\n22\n8\n48#1\n30\n"
"63#1028\n28#print\n2#       %s -p filename : print code\n27\n3\n22\n8\n48#1\n30\n35#221\n28#len\n27\n48#1\n3#2.0\n15\n37#34\n"
"63#1031\n2#repl\n1#1\n"
"63#1032\n2#mp_opcode\n1#1\n"
"63#1033\n28#mp_opcode\n2#opcodes\n22\n25#1\n"
"63#1034\n27\n3#1.0\n22\n25#2\n"
"63#1036\n28#Compiler\n48\n25#3\n"
"63#1037\n27#3\n2#compile_to_c_code\n22\n27#2\n48#1\n25#4\n"
"63#1038\n28#print\n27#4\n48#1\n30\n35#182\n28#len\n27\n48#1\n3#3.0\n13\n37#176\n"
"63#1040\n28#dict\n48\n25#5\n"
"63#1041\n28#ArgReader\n27\n3#1.0\n24\n23\n48#1\n25#6\n"
"63#1042\n27#6\n2#has_next\n22\n48\n37#157\n"
"63#1043\n27#6\n2#next\n22\n48\n25#7\n"
"63#1044\n27#7\n2#-p\n15\n37#29\n"
"63#1045\n28#assert\n27#6\n2#has_next\n22\n48\n2#expect filename\n48#2\n30\n"
"63#1046\n27#6\n2#next\n22\n48\n25#2\n"
"63#1047\n28#compilefile\n27#2\n48#1\n25#4\n"
"63#1048\n28#print\n27#4\n48#1\n30\n24\n52\n35#1\n"
"63#1051\n27#7\n2#-dis\n15\n37#24\n"
"63#1052\n28#assert\n27#6\n2#has_next\n22\n48\n2#expect filename\n48#2\n30\n"
"63#1053\n27#6\n2#next\n22\n48\n25#2\n"
"63#1054\n28#dis\n27#2\n48#1\n30\n24\n52\n35#1\n"
"63#1057\n27#7\n2#-const_name\n15\n37#20\n"
"63#1058\n28#assert\n27#6\n2#has_next\n22\n48\n48#1\n30\n"
"63#1059\n27#6\n2#next\n22\n48\n27#5\n2#const_name\n21\n"
"63#1060\n36#94\n35#1\n"
"63#1062\n27#7\n2#-c_code\n15\n37#9\n"
"63#1063\n28#True\n27#5\n2#c_code\n21\n"
"63#1064\n36#107\n35#1\n"
"63#1066\n2#c_code\n27#5\n17\n37#41\n"
"63#1067\n27#7\n25#2\n"
"63#1068\n27#2\n2#split\n22\n2#.\n48#1\n3\n22\n2#_bin\n4\n25#8\n"
"63#1069\n27#5\n2#get\n22\n2#const_name\n27#8\n48#2\n25#9\n"
"63#1071\n28#Compiler\n48\n25#10\n"
"63#1072\n27#10\n2#compile_to_c_code\n22\n27#2\n27#9\n48#2\n25#4\n"
"63#1073\n28#print\n27#4\n48#1\n30\n35#7\n"
"63#1075\n28#compile\n27#7\n2##test\n48#2\n30\n36#160\n35#1\n53\n26#main\n"
"63#1077\n28#__name__\n2#__main__\n15\n37#6\n"
"63#1078\n28#main\n48\n30\n35#1\n61\n";
