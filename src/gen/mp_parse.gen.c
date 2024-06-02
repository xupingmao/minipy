const char* mp_parse_bin="31\n2#number\n32\n2#string\n32\n2#name\n32\n2#None\n32\n26##1\n31\n2#=\n32\n2#+=\n32\n2#-=\n32\n2#*=\n32\n2#/=\n32\n2#%=\n32\n26##2\n31\n2#>\n32\n2#<\n32\n2#==\n32\n2#is\n32\n2#!=\n32\n2#>=\n32\n2#<=\n32\n2#in\n32\n2#notin\n32\n2#isnot\n32\n26##3\n31\n2#+\n32\n2#-\n32\n26##4\n31\n2#*\n32\n2#/\n32\n2#%\n32\n26##5\n31\n2#.\n32\n2#(\n32\n2#[\n32\n26##6\n31\n2#nl\n32\n2#eof\n32\n26##7\n31\n2#number\n32\n2#string\n32\n2#None\n32\n2#name\n32\n26##8\n69#\n"
"63#7\n2#mp_tokenize\n2#*\n1#2\n"
"63#9\n2#tm\n28#globals\n48\n17\n10\n37#6\n"
"63#10\n2#boot\n2#*\n1#2\n35#1\n"
"63#12\n31\n2#nl\n32\n2#dedent\n32\n26#_smp_end_list\n"
"63#13\n31\n2#nl\n32\n2#;\n32\n26#_skip_op\n"
"63#16\n54#AstNode\n51#__init__\n24\n25#1\n24\n25#2\n24\n25#3\n"
"63#18\n57#259\n"
"63#19\n27#1\n27\n2#type\n21\n"
"63#20\n27#2\n27\n2#first\n21\n"
"63#21\n27#3\n27\n2#second\n21\n53\n28#AstNode\n2#__init__\n21\n56\n"
"63#23\n54#ParserCtx\n51#__init__\n"
"63#24\n57#768\n"
"63#26\n28#Token\n2#nl\n2#nl\n24\n48#3\n27\n2#token\n21\n"
"63#27\n28#Token\n2#eof\n2#eof\n24\n48#3\n27\n2#eof\n21\n"
"63#28\n27#1\n2#append\n22\n27\n2#token\n22\n48#1\n30\n"
"63#29\n27#1\n2#append\n22\n27\n2#eof\n22\n48#1\n30\n"
"63#30\n27#1\n27\n2#tokens\n21\n"
"63#31\n27#1\n27\n2#r\n21\n"
"63#32\n3\n27\n2#i\n21\n"
"63#33\n28#len\n27#1\n48#1\n27\n2#l\n21\n"
"63#34\n31\n27\n2#tree\n21\n"
"63#35\n27#2\n27\n2#src\n21\n"
"63#36\n24\n27\n2#last_token\n21\n53\n28#ParserCtx\n2#__init__\n21\n51#next\n"
"63#38\n57#256\n"
"63#39\n27\n2#i\n22\n27\n2#l\n22\n12\n37#22\n"
"63#40\n27\n2#r\n22\n27\n2#i\n22\n22\n27\n2#token\n21\n"
"63#41\n27\n2#i\n22\n3#1.0\n4\n27\n2#i\n21\n35#8\n"
"63#43\n27\n2#eof\n22\n27\n2#token\n21\n53\n28#ParserCtx\n2#next\n21\n51#pop\n"
"63#45\n57#256\n"
"63#46\n27\n2#tree\n22\n2#pop\n22\n49\n52\n53\n28#ParserCtx\n2#pop\n21\n51#last\n"
"63#48\n57#256\n"
"63#49\n27\n2#tree\n22\n3#-1.0\n22\n52\n53\n28#ParserCtx\n2#last\n21\n51#add\n"
"63#51\n57#512\n"
"63#52\n27\n2#tree\n22\n2#append\n22\n27#1\n48#1\n30\n"
"63#53\n27#1\n27\n2#last_token\n21\n53\n28#ParserCtx\n2#add\n21\n51#add_op\n"
"63#55\n57#512\n"
"63#56\n27\n2#pop\n22\n48\n25#2\n"
"63#57\n27\n2#pop\n22\n48\n25#3\n"
"63#58\n27\n2#add\n22\n28#AstNode\n27#1\n27#3\n27#2\n48#3\n48#1\n30\n53\n28#ParserCtx\n2#add_op\n21\n51#visit_block\n"
"63#60\n57#256\n"
"63#61\n27\n2#tree\n22\n2#append\n22\n2#block\n48#1\n30\n"
"63#62\n28#parse_block\n27\n48#1\n30\n"
"63#63\n31\n25#1\n"
"63#64\n27\n2#tree\n22\n2#pop\n22\n48\n25#2\n"
"63#65\n27#2\n2#block\n16\n37#17\n"
"63#66\n27#1\n2#append\n22\n27#2\n48#1\n30\n"
"63#67\n27\n2#tree\n22\n2#pop\n22\n48\n25#2\n36#19\n"
"63#68\n27#1\n2#reverse\n22\n48\n30\n"
"63#69\n27#1\n52\n53\n28#ParserCtx\n2#visit_block\n21\n56\n"
"63#71\n51#expect_and_next\n24\n25#2\n"
"63#71\n57#513\n"
"63#73\n27\n2#token\n22\n2#type\n22\n27#1\n16\n37#29\n"
"63#74\n2#expect %r but now is %r::%s\n31\n27#1\n32\n27\n2#token\n22\n2#type\n22\n32\n27#2\n32\n8\n25#3\n"
"63#75\n28#compile_error\n2#parse\n27\n2#src\n22\n27\n2#token\n22\n27#3\n48#4\n30\n35#1\n"
"63#76\n27\n2#next\n22\n48\n30\n53\n26#expect_and_next\n"
"63#78\n51#assert_type\n"
"63#78\n57#768\n"
"63#79\n27\n2#token\n22\n2#type\n22\n27#1\n16\n37#11\n"
"63#80\n28#parse_error\n27\n27\n2#token\n22\n27#2\n48#3\n30\n35#1\n53\n26#assert_type\n"
"63#83\n51#add_op\n"
"63#83\n57#512\n"
"63#84\n27\n2#tree\n22\n2#pop\n22\n48\n25#2\n"
"63#85\n27\n2#tree\n22\n2#pop\n22\n48\n25#3\n"
"63#86\n27\n2#add\n22\n28#AstNode\n27#1\n27#3\n27#2\n48#3\n48#1\n30\n53\n26#add_op\n"
"63#88\n51#build_op\n"
"63#88\n57#512\n"
"63#89\n27\n2#tree\n22\n2#pop\n22\n48\n25#2\n"
"63#90\n27\n2#tree\n22\n2#pop\n22\n48\n25#3\n"
"63#91\n28#AstNode\n27#1\n27#3\n27#2\n49#3\n52\n53\n26#build_op\n"
"63#94\n51#parse_error\n24\n25#1\n2#Unknown\n25#2\n"
"63#94\n57#258\n"
"63#95\n27#1\n24\n16\n37#12\n"
"63#96\n28#compile_error\n2#parse\n27\n2#src\n22\n27#1\n27#2\n48#4\n30\n35#6\n"
"63#98\n28#raise\n2#assert_type error\n48#1\n30\n53\n26#parse_error\n"
"63#100\n51#baseitem\n"
"63#100\n57#256\n"
"63#101\n27\n2#token\n22\n2#type\n22\n25#1\n"
"63#102\n27\n2#token\n22\n25#2\n"
"63#103\n27#1\n28##1\n17\n37#15\n"
"63#104\n27\n2#next\n22\n48\n30\n"
"63#105\n27\n2#add\n22\n27#2\n48#1\n30\n35#250\n27#1\n2#[\n15\n37#68\n"
"63#107\n27\n2#next\n22\n48\n30\n"
"63#108\n28#AstNode\n2#list\n48#1\n25#3\n"
"63#109\n27#2\n2#pos\n22\n27#3\n2#pos\n21\n"
"63#110\n27\n2#token\n22\n2#type\n22\n2#]\n15\n37#13\n"
"63#111\n27\n2#next\n22\n48\n30\n"
"63#112\n24\n27#3\n2#first\n21\n35#21\n"
"63#114\n28#exp\n27\n2#,\n48#2\n30\n"
"63#115\n28#expect_and_next\n27\n2#]\n48#2\n30\n"
"63#116\n27\n2#pop\n22\n48\n27#3\n2#first\n21\n"
"63#117\n27\n2#add\n22\n27#3\n48#1\n30\n35#179\n27#1\n2#(\n15\n37#53\n"
"63#119\n27\n2#next\n22\n48\n30\n"
"63#120\n28#exp\n27\n2#,\n48#2\n30\n"
"63#121\n28#expect_and_next\n27\n2#)\n48#2\n30\n"
"63#123\n27\n2#last\n22\n48\n25#4\n"
"63#124\n28#gettype\n27#4\n48#1\n2#list\n15\n37#21\n"
"63#125\n27\n2#pop\n22\n48\n30\n"
"63#126\n28#AstNode\n2#tuple\n27#4\n48#2\n25#3\n"
"63#127\n27\n2#add\n22\n27#3\n48#1\n30\n35#1\n35#123\n27#1\n2#{\n15\n37#119\n"
"63#129\n27\n2#next\n22\n48\n30\n"
"63#130\n28#AstNode\n2#dict\n48#1\n25#3\n"
"63#131\n31\n25#5\n"
"63#132\n27\n2#token\n22\n2#type\n22\n2#}\n16\n37#61\n"
"63#133\n28#exp\n27\n2#or\n48#2\n30\n"
"63#134\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#135\n28#exp\n27\n2#or\n48#2\n30\n"
"63#137\n27\n2#pop\n22\n48\n25#6\n"
"63#138\n27\n2#pop\n22\n48\n25#7\n"
"63#139\n27#5\n2#append\n22\n31\n27#7\n32\n27#6\n32\n48#1\n30\n"
"63#140\n27\n2#token\n22\n2#type\n22\n2#}\n15\n37#4\n"
"63#141\n35#9\n35#1\n"
"63#142\n28#expect_and_next\n27\n2#,\n48#2\n30\n36#67\n"
"63#143\n27\n2#token\n22\n2#type\n22\n2#,\n15\n37#8\n"
"63#144\n27\n2#next\n22\n48\n30\n35#1\n"
"63#145\n28#expect_and_next\n27\n2#}\n48#2\n30\n"
"63#146\n27#5\n27#3\n2#first\n21\n"
"63#147\n27\n2#add\n22\n27#3\n48#1\n30\n35#1\n53\n26#baseitem\n"
"63#149\n51#expr\n"
"63#149\n57#256\n"
"63#150\n28#exp\n27\n2#=\n49#2\n52\n53\n26#expr\n"
"63#152\n51#parse_assign_or_exp\n"
"63#152\n57#256\n"
"63#153\n28#exp\n27\n2#=\n49#2\n52\n53\n26#parse_assign_or_exp\n"
"63#155\n51#parse_rvalue\n"
"63#155\n57#256\n"
"63#156\n28#exp\n27\n2#rvalue\n49#2\n52\n53\n26#parse_rvalue\n"
"63#158\n51#parse_var\n"
"63#158\n57#256\n"
"63#159\n27\n2#token\n22\n25#1\n"
"63#160\n28#expect_and_next\n27\n2#name\n48#2\n30\n"
"63#161\n27\n2#add\n22\n27#1\n48#1\n30\n"
"63#163\n28#True\n37#63\n"
"63#164\n27\n2#token\n22\n2#type\n22\n2#.\n15\n37#14\n"
"63#165\n28#parse_var\n27\n48#1\n30\n"
"63#166\n27\n2#add_op\n22\n2#attr\n48#1\n30\n35#40\n27\n2#token\n22\n2#type\n22\n2#[\n15\n37#26\n"
"63#168\n27\n2#next\n22\n48\n30\n"
"63#169\n28#parse_rvalue\n27\n48#1\n30\n"
"63#170\n28#expect_and_next\n27\n2#]\n48#2\n30\n"
"63#171\n27\n2#add_op\n22\n2#get\n48#1\n30\n35#7\n"
"63#173\n27\n2#pop\n22\n49\n52\n36#63\n53\n26#parse_var\n"
"63#175\n51#parse_var_list\n"
"63#175\n57#256\n"
"63#177\n31\n28#parse_var\n27\n48#1\n32\n25#1\n"
"63#178\n27\n2#token\n22\n2#type\n22\n2#,\n15\n37#14\n"
"63#179\n28#parse_var\n27\n48#1\n25#2\n"
"63#180\n27#1\n2#append\n22\n27#2\n48#1\n30\n36#20\n"
"63#182\n28#len\n27#1\n48#1\n3#1.0\n11\n37#9\n"
"63#183\n27\n2#add\n22\n27#1\n48#1\n30\n35#10\n"
"63#185\n27\n2#add\n22\n27#1\n3\n22\n48#1\n30\n53\n26#parse_var_list\n"
"63#187\n51#parse_name_list\n"
"63#187\n57#256\n"
"63#190\n31\n28#parse_name\n27\n48#1\n32\n25#1\n"
"63#191\n27\n2#token\n22\n2#type\n22\n2#,\n15\n37#20\n"
"63#192\n27\n2#next\n22\n48\n30\n"
"63#193\n28#parse_name\n27\n48#1\n25#2\n"
"63#194\n27#1\n2#append\n22\n27#2\n48#1\n30\n36#26\n"
"63#195\n27#1\n52\n53\n26#parse_name_list\n"
"63#197\n51#parse_name\n"
"63#197\n57#256\n"
"63#198\n27\n2#token\n22\n25#1\n"
"63#199\n28#expect_and_next\n27\n2#name\n48#2\n30\n"
"63#200\n27#1\n52\n53\n26#parse_name\n"
"63#210\n51#exp\n"
"63#210\n57#512\n"
"63#212\n27#1\n2#=\n15\n37#43\n"
"63#214\n28#exp\n27\n2#,\n48#2\n30\n"
"63#215\n27\n2#token\n22\n2#type\n22\n28##2\n17\n37#27\n"
"63#216\n27\n2#token\n22\n2#type\n22\n25#2\n"
"63#217\n27\n2#next\n22\n48\n30\n"
"63#218\n28#exp\n27\n2#,\n48#2\n30\n"
"63#219\n28#add_op\n27\n27#2\n48#2\n30\n35#1\n35#374\n27#1\n2#,\n15\n39#5\n27#1\n2#rvalue\n15\n20\n37#85\n"
"63#221\n28#exp\n27\n2#or\n48#2\n30\n"
"63#222\n24\n25#3\n"
"63#223\n27\n2#token\n22\n2#type\n22\n2#,\n15\n37#53\n"
"63#224\n27#3\n24\n15\n37#10\n"
"63#225\n31\n27\n2#pop\n22\n48\n32\n25#3\n35#1\n"
"63#226\n27\n2#next\n22\n48\n30\n"
"63#227\n27\n2#token\n22\n2#type\n22\n2#]\n15\n37#4\n"
"63#228\n35#22\n35#1\n"
"63#229\n28#exp\n27\n2#or\n48#2\n30\n"
"63#230\n27\n2#pop\n22\n48\n25#4\n"
"63#231\n27#3\n2#append\n22\n27#4\n48#1\n30\n36#59\n"
"63#232\n27#3\n24\n16\n37#9\n"
"63#233\n27\n2#add\n22\n27#3\n48#1\n30\n35#1\n35#281\n27#1\n2#or\n15\n37#36\n"
"63#235\n28#exp\n27\n2#and\n48#2\n30\n"
"63#236\n27\n2#token\n22\n2#type\n22\n2#or\n15\n37#20\n"
"63#237\n27\n2#next\n22\n48\n30\n"
"63#238\n28#exp\n27\n2#and\n48#2\n30\n"
"63#239\n28#add_op\n27\n2#or\n48#2\n30\n36#26\n35#242\n27#1\n2#and\n15\n37#36\n"
"63#241\n28#exp\n27\n2#not\n48#2\n30\n"
"63#242\n27\n2#token\n22\n2#type\n22\n2#and\n15\n37#20\n"
"63#243\n27\n2#next\n22\n48\n30\n"
"63#244\n28#exp\n27\n2#not\n48#2\n30\n"
"63#245\n28#add_op\n27\n2#and\n48#2\n30\n36#26\n35#203\n27#1\n2#not\n15\n37#60\n"
"63#247\n27\n2#token\n22\n2#type\n22\n2#not\n15\n37#44\n"
"63#248\n27\n2#token\n22\n2#type\n22\n2#not\n15\n37#34\n"
"63#249\n27\n2#next\n22\n48\n30\n"
"63#250\n28#exp\n27\n2#not\n48#2\n30\n"
"63#251\n28#AstNode\n2#not\n48#1\n25#5\n"
"63#252\n27\n2#pop\n22\n48\n27#5\n2#first\n21\n"
"63#253\n27\n2#add\n22\n27#5\n48#1\n30\n36#40\n35#7\n"
"63#255\n28#exp\n27\n2#cmp\n48#2\n30\n35#140\n27#1\n2#cmp\n15\n37#43\n"
"63#257\n28#exp\n27\n2#+-\n48#2\n30\n"
"63#258\n27\n2#token\n22\n2#type\n22\n28##3\n17\n37#27\n"
"63#260\n27\n2#token\n22\n2#type\n22\n25#2\n"
"63#261\n27\n2#next\n22\n48\n30\n"
"63#262\n28#exp\n27\n2#+-\n48#2\n30\n"
"63#263\n28#add_op\n27\n27#2\n48#2\n30\n36#33\n35#94\n27#1\n2#+-\n15\n37#43\n"
"63#265\n28#exp\n27\n2#factor\n48#2\n30\n"
"63#266\n27\n2#token\n22\n2#type\n22\n28##4\n17\n37#27\n"
"63#267\n27\n2#token\n22\n2#type\n22\n25#2\n"
"63#268\n27\n2#next\n22\n48\n30\n"
"63#269\n28#exp\n27\n2#factor\n48#2\n30\n"
"63#270\n28#add_op\n27\n27#2\n48#2\n30\n36#33\n35#48\n27#1\n2#factor\n15\n37#44\n"
"63#272\n28#call_or_get_exp\n25#6\n"
"63#273\n27#6\n27\n48#1\n30\n"
"63#274\n27\n2#token\n22\n2#type\n22\n28##5\n17\n37#26\n"
"63#275\n27\n2#token\n22\n2#type\n22\n25#2\n"
"63#276\n27\n2#next\n22\n48\n30\n"
"63#277\n27#6\n27\n48#1\n30\n"
"63#278\n28#add_op\n27\n27#2\n48#2\n30\n36#32\n35#1\n53\n26#exp\n"
"63#280\n51#parse_arg_list\n"
"63#280\n57#256\n"
"63#281\n28#AstNode\n2#call\n27\n2#pop\n22\n48\n48#2\n25#1\n"
"63#282\n27\n2#token\n22\n2#type\n22\n2#)\n15\n37#16\n"
"63#283\n27\n2#next\n22\n48\n30\n"
"63#284\n24\n27#1\n2#second\n21\n"
"63#285\n27#1\n52\n35#1\n"
"63#286\n31\n25#2\n"
"63#287\n27\n2#token\n22\n2#type\n22\n2#)\n16\n37#79\n"
"63#288\n27\n2#token\n22\n2#type\n22\n2#*\n15\n37#34\n"
"63#289\n27\n2#next\n22\n48\n30\n"
"63#290\n28#exp\n27\n2#or\n48#2\n30\n"
"63#291\n27\n2#pop\n22\n48\n25#3\n"
"63#292\n27#2\n2#append\n22\n27#3\n48#1\n30\n"
"63#293\n2#apply\n27#1\n2#type\n21\n"
"63#294\n35#38\n35#36\n"
"63#296\n28#exp\n27\n2#or\n48#2\n30\n"
"63#297\n27\n2#pop\n22\n48\n25#3\n"
"63#298\n27#2\n2#append\n22\n27#3\n48#1\n30\n"
"63#299\n27\n2#token\n22\n2#type\n22\n2#,\n15\n37#8\n"
"63#300\n27\n2#next\n22\n48\n30\n35#1\n36#85\n"
"63#301\n28#expect_and_next\n27\n2#)\n48#2\n30\n"
"63#302\n27#2\n27#1\n2#second\n21\n"
"63#304\n27#1\n52\n53\n26#parse_arg_list\n"
"63#306\n51#call_or_get_exp\n"
"63#306\n57#256\n"
"63#307\n27\n2#token\n22\n2#type\n22\n2#-\n15\n37#29\n"
"63#308\n27\n2#next\n22\n48\n30\n"
"63#309\n28#call_or_get_exp\n27\n48#1\n30\n"
"63#310\n28#AstNode\n2#neg\n27\n2#pop\n22\n48\n48#2\n25#1\n"
"63#311\n27\n2#add\n22\n27#1\n48#1\n30\n35#236\n"
"63#313\n28#baseitem\n27\n48#1\n30\n"
"63#314\n27\n2#token\n22\n2#type\n22\n28##6\n17\n37#222\n"
"63#315\n27\n2#token\n22\n2#type\n22\n25#2\n"
"63#316\n27#2\n2#[\n15\n37#141\n"
"63#317\n27\n2#next\n22\n48\n30\n"
"63#318\n27\n2#pop\n22\n48\n25#3\n"
"63#319\n24\n25#4\n"
"63#320\n24\n25#5\n"
"63#321\n27\n2#token\n22\n2#type\n22\n2#:\n15\n37#13\n"
"63#322\n28#Token\n2#number\n3\n27\n2#token\n22\n2#pos\n22\n48#3\n25#4\n35#13\n"
"63#324\n28#exp\n27\n2#or\n48#2\n30\n"
"63#325\n27\n2#pop\n22\n48\n25#4\n"
"63#327\n27\n2#token\n22\n2#type\n22\n2#:\n15\n37#35\n"
"63#328\n27\n2#next\n22\n48\n30\n"
"63#329\n27\n2#token\n22\n2#type\n22\n2#]\n15\n37#7\n"
"63#330\n28#Token\n2#None\n48#1\n25#5\n35#13\n"
"63#332\n28#exp\n27\n2#or\n48#2\n30\n"
"63#333\n27\n2#pop\n22\n48\n25#5\n35#1\n"
"63#334\n28#expect_and_next\n27\n2#]\n48#2\n30\n"
"63#335\n27#5\n24\n15\n37#16\n"
"63#336\n28#AstNode\n2#get\n27#3\n27#4\n48#3\n25#1\n"
"63#337\n27\n2#add\n22\n27#1\n48#1\n30\n35#20\n"
"63#339\n28#AstNode\n2#slice\n27#3\n27#4\n48#3\n25#1\n"
"63#340\n27#5\n27#1\n2#third\n21\n"
"63#341\n27\n2#add\n22\n27#1\n48#1\n30\n35#69\n27#2\n2#(\n15\n37#20\n"
"63#343\n27\n2#next\n22\n48\n30\n"
"63#344\n28#parse_arg_list\n27\n48#1\n25#1\n"
"63#345\n27\n2#add\n22\n27#1\n48#1\n30\n35#46\n"
"63#347\n27\n2#next\n22\n48\n30\n"
"63#348\n28#baseitem\n27\n48#1\n30\n"
"63#349\n27\n2#pop\n22\n48\n25#6\n"
"63#350\n27\n2#pop\n22\n48\n25#7\n"
"63#351\n28#AstNode\n2#attr\n48#1\n25#1\n"
"63#352\n27#7\n27#1\n2#first\n21\n"
"63#353\n27#6\n27#1\n2#second\n21\n"
"63#354\n27\n2#add\n22\n27#1\n48#1\n30\n36#228\n53\n26#call_or_get_exp\n"
"63#356\n51#_get_path\n"
"63#356\n57#256\n"
"63#357\n27\n2#type\n22\n25#1\n"
"63#358\n27#1\n2#get\n15\n37#17\n"
"63#359\n28#_get_path\n27\n2#first\n22\n48#1\n2#/\n4\n28#_get_path\n27\n2#second\n22\n48#1\n4\n52\n35#25\n27#1\n2#name\n15\n37#7\n"
"63#362\n27\n2#val\n22\n52\n35#15\n27#1\n2#string\n15\n37#7\n"
"63#364\n27\n2#val\n22\n52\n35#5\n"
"63#366\n28#raise\n48\n30\n53\n26#_get_path\n"
"63#368\n51#_path_check\n"
"63#368\n57#512\n"
"63#369\n27#1\n2#type\n22\n2#attr\n15\n37#18\n"
"63#370\n28#_path_check\n27\n27#1\n2#first\n22\n48#2\n30\n"
"63#371\n28#_path_check\n27\n27#1\n2#second\n22\n48#2\n30\n35#18\n27#1\n2#type\n22\n2#name\n15\n37#5\n"
"63#373\n28#True\n52\n35#8\n"
"63#375\n28#parse_error\n27\n27#1\n2#import error\n48#3\n30\n53\n26#_path_check\n"
"63#377\n51#_name_check\n"
"63#377\n57#512\n"
"63#378\n27#1\n2#type\n22\n2#,\n15\n37#18\n"
"63#379\n28#_name_check\n27\n27#1\n2#first\n22\n48#2\n30\n"
"63#380\n28#_name_check\n27\n27#1\n2#second\n22\n48#2\n30\n35#22\n27#1\n2#type\n22\n2#name\n15\n37#5\n"
"63#382\n28#True\n52\n35#12\n"
"63#384\n28#parse_error\n27\n27#1\n2#import error\n27#1\n2#type\n22\n4\n48#3\n30\n53\n26#_name_check\n"
"63#386\n51#parse_from\n"
"63#386\n57#256\n"
"63#388\n28#expect_and_next\n27\n2#from\n48#2\n30\n"
"63#389\n28#expr\n27\n48#1\n30\n"
"63#390\n28#expect_and_next\n27\n2#import\n48#2\n30\n"
"63#391\n28#AstNode\n2#from\n48#1\n25#1\n"
"63#392\n27\n2#pop\n22\n48\n27#1\n2#first\n21\n"
"63#393\n28#_path_check\n27\n27#1\n2#first\n22\n48#2\n30\n"
"63#395\n27\n2#token\n22\n2#type\n22\n2#*\n15\n37#16\n"
"63#396\n2#string\n27\n2#token\n22\n2#type\n21\n"
"63#397\n27\n2#token\n22\n27#1\n2#second\n21\n35#28\n"
"63#399\n28#parse_name_list\n27\n48#1\n25#2\n"
"63#400\n28#len\n27#2\n48#1\n3#1.0\n16\n37#9\n"
"63#401\n28#raise\n28#Exception\n2#from statement can only import one attribute\n48#1\n48#1\n30\n35#1\n"
"63#402\n27#2\n3\n22\n27#1\n2#second\n21\n"
"63#404\n27\n2#next\n22\n48\n30\n"
"63#406\n27\n2#add\n22\n27#1\n48#1\n30\n53\n26#parse_from\n"
"63#408\n51#parse_import\n"
"63#408\n57#256\n"
"63#409\n27\n2#next\n22\n48\n30\n"
"63#410\n28#expr\n27\n48#1\n30\n"
"63#411\n28#AstNode\n2#import\n48#1\n25#1\n"
"63#412\n27\n2#pop\n22\n48\n27#1\n2#first\n21\n"
"63#413\n27\n2#add\n22\n27#1\n48#1\n30\n53\n26#parse_import\n"
"63#416\n51#skip_nl\n"
"63#416\n57#256\n"
"63#417\n27\n2#token\n22\n2#type\n22\n28#_skip_op\n17\n37#8\n"
"63#418\n27\n2#next\n22\n48\n30\n36#14\n53\n26#skip_nl\n"
"63#420\n51#call_node\n"
"63#420\n57#512\n"
"63#421\n28#AstNode\n2#call\n48#1\n25#2\n"
"63#422\n27\n27#2\n2#first\n21\n"
"63#423\n27#1\n27#2\n2#second\n21\n"
"63#424\n27#2\n52\n53\n26#call_node\n"
"63#426\n51#parse_inner_func\n"
"63#426\n57#256\n"
"63#427\n27\n2#token\n22\n25#1\n"
"63#428\n2#name\n27#1\n2#type\n21\n"
"63#429\n27\n2#next\n22\n48\n30\n"
"63#430\n27\n2#token\n22\n2#type\n22\n2#nl\n15\n37#5\n"
"63#431\n24\n25#2\n35#13\n"
"63#433\n28#exp\n27\n2#,\n48#2\n30\n"
"63#434\n27\n2#pop\n22\n48\n25#2\n"
"63#435\n27\n2#add\n22\n28#call_node\n27#1\n27#2\n48#2\n48#1\n30\n53\n26#parse_inner_func\n"
"63#437\n51#parse_del\n"
"63#437\n57#256\n"
"63#438\n27\n2#next\n22\n48\n30\n"
"63#439\n28#expr\n27\n48#1\n30\n"
"63#440\n28#AstNode\n2#del\n27\n2#pop\n22\n48\n48#2\n25#1\n"
"63#441\n27\n2#add\n22\n27#1\n48#1\n30\n53\n26#parse_del\n"
"63#443\n51#parse_global\n"
"63#443\n57#256\n"
"63#444\n27\n2#next\n22\n48\n30\n"
"63#445\n28#AstNode\n2#global\n48#1\n25#1\n"
"63#446\n28#assert_type\n27\n2#name\n2#Global_exception\n48#3\n30\n"
"63#447\n27\n2#token\n22\n27#1\n2#first\n21\n"
"63#448\n27\n2#add\n22\n27#1\n48#1\n30\n"
"63#449\n27\n2#next\n22\n48\n30\n53\n26#parse_global\n"
"63#451\n51#parse_pass\n"
"63#451\n57#256\n"
"63#452\n27\n2#token\n22\n25#1\n"
"63#453\n27\n2#next\n22\n48\n30\n"
"63#454\n28#AstNode\n27#1\n2#type\n22\n48#1\n25#2\n"
"63#455\n27#1\n2#pos\n22\n27#2\n2#pos\n21\n"
"63#456\n27\n2#add\n22\n27#2\n48#1\n30\n53\n26#parse_pass\n"
"63#458\n51#parse_try\n"
"63#458\n57#256\n"
"63#459\n27\n2#token\n22\n2#pos\n22\n25#1\n"
"63#460\n27\n2#next\n22\n48\n30\n"
"63#461\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#462\n28#AstNode\n2#try\n27\n2#visit_block\n22\n48\n48#2\n25#2\n"
"63#463\n27#1\n27#2\n2#pos\n21\n"
"63#464\n28#expect_and_next\n27\n2#except\n48#2\n30\n"
"63#465\n27\n2#token\n22\n2#type\n22\n2#name\n15\n37#47\n"
"63#466\n27\n2#next\n22\n48\n30\n"
"63#467\n27\n2#token\n22\n2#type\n22\n2#:\n15\n37#8\n"
"63#468\n28#Token\n2#name\n2#_\n48#2\n25#3\n35#19\n"
"63#470\n28#expect_and_next\n27\n2#as\n48#2\n30\n"
"63#471\n27\n2#token\n22\n25#3\n"
"63#472\n28#expect_and_next\n27\n2#name\n2#error in try-expression\n48#3\n30\n"
"63#473\n27#3\n27#2\n2#second\n21\n35#6\n"
"63#474\n24\n27#2\n2#second\n21\n"
"63#475\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#476\n27\n2#visit_block\n22\n48\n27#2\n2#third\n21\n"
"63#477\n27\n2#add\n22\n27#2\n48#1\n30\n53\n26#parse_try\n"
"63#479\n51#parse_for_items\n"
"63#479\n57#256\n"
"63#480\n27\n2#token\n22\n25#1\n"
"63#481\n28#expect_and_next\n27\n2#name\n48#2\n30\n"
"63#482\n31\n27#1\n32\n25#2\n"
"63#483\n27\n2#token\n22\n2#type\n22\n2#,\n15\n37#26\n"
"63#484\n27\n2#next\n22\n48\n30\n"
"63#485\n27\n2#token\n22\n25#1\n"
"63#486\n28#expect_and_next\n27\n2#name\n48#2\n30\n"
"63#487\n27#2\n2#append\n22\n27#1\n48#1\n30\n36#32\n"
"63#488\n28#expect_and_next\n27\n2#in\n48#2\n30\n"
"63#489\n28#expr\n27\n48#1\n30\n"
"63#490\n28#AstNode\n2#in\n48#1\n25#3\n"
"63#491\n27#2\n27#3\n2#first\n21\n"
"63#492\n27\n2#pop\n22\n48\n27#3\n2#second\n21\n"
"63#493\n27#3\n52\n53\n26#parse_for_items\n"
"63#496\n51#parse_for\n"
"63#496\n57#256\n"
"63#498\n28#AstNode\n2#for\n48#1\n25#1\n"
"63#499\n27\n2#next\n22\n48\n30\n"
"63#500\n28#parse_for_items\n27\n48#1\n25#2\n"
"63#501\n27#2\n27#1\n2#first\n21\n"
"63#502\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#503\n27\n2#visit_block\n22\n48\n27#1\n2#second\n21\n"
"63#504\n27\n2#add\n22\n27#1\n48#1\n30\n53\n26#parse_for\n"
"63#506\n51#parse_while\n"
"63#506\n57#256\n"
"63#507\n28#parse_for_while\n27\n2#while\n48#2\n30\n53\n26#parse_while\n"
"63#510\n51#parse_for_while\n"
"63#510\n57#512\n"
"63#511\n28#AstNode\n27#1\n48#1\n25#2\n"
"63#512\n27\n2#next\n22\n48\n30\n"
"63#513\n28#expr\n27\n48#1\n30\n"
"63#514\n27\n2#pop\n22\n48\n27#2\n2#first\n21\n"
"63#515\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#516\n27\n2#visit_block\n22\n48\n27#2\n2#second\n21\n"
"63#517\n27\n2#add\n22\n27#2\n48#1\n30\n53\n26#parse_for_while\n"
"63#519\n51#parse_arg_def\n"
"63#519\n57#256\n"
"63#520\n28#expect_and_next\n27\n2#(\n48#2\n30\n"
"63#521\n27\n2#token\n22\n2#type\n22\n2#)\n15\n37#11\n"
"63#522\n27\n2#next\n22\n48\n30\n"
"63#523\n31\n52\n35#179\n"
"63#525\n31\n25#1\n"
"63#531\n3\n25#2\n"
"63#532\n27\n2#token\n22\n2#type\n22\n2#name\n15\n37#106\n"
"63#533\n28#AstNode\n2#arg\n48#1\n25#3\n"
"63#534\n27\n2#token\n22\n27#3\n2#first\n21\n"
"63#535\n24\n27#3\n2#second\n21\n"
"63#536\n27\n2#next\n22\n48\n30\n"
"63#537\n27#2\n3#1.0\n15\n37#21\n"
"63#538\n28#expect_and_next\n27\n2#=\n48#2\n30\n"
"63#539\n28#baseitem\n27\n48#1\n30\n"
"63#540\n27\n2#pop\n22\n48\n27#3\n2#second\n21\n35#32\n27\n2#token\n22\n2#type\n22\n2#=\n15\n37#24\n"
"63#542\n27\n2#next\n22\n48\n30\n"
"63#543\n28#baseitem\n27\n48#1\n30\n"
"63#544\n27\n2#pop\n22\n48\n27#3\n2#second\n21\n"
"63#545\n3#1.0\n25#2\n35#1\n"
"63#546\n27#1\n2#append\n22\n27#3\n48#1\n30\n"
"63#547\n27\n2#token\n22\n2#type\n22\n2#,\n16\n37#4\n"
"63#547\n35#9\n35#1\n"
"63#548\n27\n2#next\n22\n48\n30\n36#112\n"
"63#549\n27\n2#token\n22\n2#type\n22\n2#*\n15\n37#41\n"
"63#550\n27\n2#next\n22\n48\n30\n"
"63#551\n28#assert_type\n27\n2#name\n2#Invalid arguments\n48#3\n30\n"
"63#552\n28#AstNode\n2#narg\n27\n2#token\n22\n48#2\n25#3\n"
"63#553\n24\n27#3\n2#second\n21\n"
"63#554\n27#1\n2#append\n22\n27#3\n48#1\n30\n"
"63#555\n27\n2#next\n22\n48\n30\n35#1\n"
"63#556\n28#expect_and_next\n27\n2#)\n48#2\n30\n"
"63#557\n27#1\n52\n53\n26#parse_arg_def\n"
"63#559\n51#parse_def\n"
"63#559\n57#256\n"
"63#560\n27\n2#next\n22\n48\n30\n"
"63#561\n28#assert_type\n27\n2#name\n2#DefException\n48#3\n30\n"
"63#562\n28#AstNode\n2#def\n48#1\n25#1\n"
"63#563\n27\n2#token\n22\n27#1\n2#first\n21\n"
"63#564\n27\n2#next\n22\n48\n30\n"
"63#565\n28#parse_arg_def\n27\n48#1\n27#1\n2#second\n21\n"
"63#566\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#567\n27\n2#visit_block\n22\n48\n27#1\n2#third\n21\n"
"63#568\n27\n2#add\n22\n27#1\n48#1\n30\n53\n26#parse_def\n"
"63#570\n51#parse_class\n"
"63#570\n57#256\n"
"63#571\n28#expect_and_next\n27\n2#class\n48#2\n30\n"
"63#573\n28#AstNode\n48\n25#1\n"
"63#574\n2#class\n27#1\n2#type\n21\n"
"63#575\n27\n2#token\n22\n27#1\n2#first\n21\n"
"63#576\n28#expect_and_next\n27\n2#name\n48#2\n30\n"
"63#578\n27\n2#token\n22\n2#type\n22\n2#(\n15\n37#34\n"
"63#579\n27\n2#next\n22\n48\n30\n"
"63#580\n28#assert_type\n27\n2#name\n2#ClassException\n48#3\n30\n"
"63#581\n27\n2#token\n22\n27\n2#third\n21\n"
"63#582\n27\n2#next\n22\n48\n30\n"
"63#583\n28#expect_and_next\n27\n2#)\n48#2\n30\n35#1\n"
"63#584\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#586\n27\n2#visit_block\n22\n48\n27#1\n2#second\n21\n"
"63#588\n28#len\n27#1\n2#second\n22\n48#1\n3\n11\n38#11\n27#1\n2#second\n22\n3\n22\n2#type\n22\n2#string\n15\n19\n37#20\n"
"63#589\n27#1\n2#second\n22\n3\n22\n25#2\n"
"63#590\n27#1\n2#second\n22\n3\n42\n"
"63#591\n27#2\n27#1\n2#doc\n21\n35#1\n"
"63#593\n27\n2#add\n22\n27#1\n48#1\n30\n53\n26#parse_class\n"
"63#595\n51#parse_stm1\n"
"63#595\n57#512\n"
"63#596\n27\n2#next\n22\n48\n30\n"
"63#597\n28#AstNode\n27#1\n48#1\n25#2\n"
"63#598\n27\n2#token\n22\n2#type\n22\n28#_smp_end_list\n17\n37#7\n"
"63#599\n24\n27#2\n2#first\n21\n35#14\n"
"63#601\n28#expr\n27\n48#1\n30\n"
"63#602\n27\n2#pop\n22\n48\n27#2\n2#first\n21\n"
"63#603\n27\n2#add\n22\n27#2\n48#1\n30\n53\n26#parse_stm1\n"
"63#607\n51#parse_if\n"
"63#607\n57#256\n"
"63#608\n28#AstNode\n2#if\n48#1\n25#1\n"
"63#609\n27\n2#next\n22\n48\n30\n"
"63#610\n28#expr\n27\n48#1\n30\n"
"63#611\n27\n2#pop\n22\n48\n27#1\n2#first\n21\n"
"63#612\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#613\n27\n2#visit_block\n22\n48\n27#1\n2#second\n21\n"
"63#614\n24\n27#1\n2#third\n21\n"
"63#615\n27#1\n25#2\n"
"63#616\n27#2\n25#3\n"
"63#617\n27\n2#token\n22\n2#type\n22\n2#elif\n15\n37#63\n"
"63#618\n27\n2#token\n22\n2#type\n22\n2#elif\n15\n37#53\n"
"63#619\n28#AstNode\n2#if\n48#1\n25#4\n"
"63#620\n27\n2#next\n22\n48\n30\n"
"63#621\n28#expr\n27\n48#1\n30\n"
"63#622\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#623\n27\n2#pop\n22\n48\n27#4\n2#first\n21\n"
"63#624\n27\n2#visit_block\n22\n48\n27#4\n2#second\n21\n"
"63#625\n24\n27#4\n2#third\n21\n"
"63#626\n27#4\n27#2\n2#third\n21\n"
"63#627\n27#4\n25#2\n36#59\n35#1\n"
"63#628\n27\n2#token\n22\n2#type\n22\n2#else\n15\n37#22\n"
"63#629\n27\n2#next\n22\n48\n30\n"
"63#630\n28#expect_and_next\n27\n2#:\n48#2\n30\n"
"63#631\n27\n2#visit_block\n22\n48\n27#2\n2#third\n21\n35#1\n"
"63#632\n27\n2#add\n22\n27#3\n48#1\n30\n53\n26#parse_if\n"
"63#634\n51#parse_return\n"
"63#634\n57#256\n"
"63#635\n28#parse_stm1\n27\n2#return\n48#2\n30\n53\n26#parse_return\n"
"63#637\n51#parse_annotation\n"
"63#637\n57#256\n"
"63#638\n27\n2#token\n22\n25#1\n"
"63#639\n27\n2#next\n22\n48\n30\n"
"63#640\n27\n2#add\n22\n28#AstNode\n2#@\n27#1\n48#2\n48#1\n30\n53\n26#parse_annotation\n"
"63#642\n51#parse_skip\n"
"63#642\n57#256\n"
"63#643\n27\n2#next\n22\n48\n30\n53\n26#parse_skip\n"
"63#645\n51#parse_multi_assign\n"
"63#645\n57#256\n"
"63#646\n27\n2#next\n22\n48\n30\n"
"63#647\n28#expr\n27\n48#1\n30\n"
"63#648\n28#expect_and_next\n27\n2#]\n48#2\n30\n"
"63#649\n28#expect_and_next\n27\n2#=\n48#2\n30\n"
"63#650\n28#expr\n27\n48#1\n30\n"
"63#651\n28#add_op\n27\n2#=\n48#2\n30\n53\n26#parse_multi_assign\n"
"63#654\n33\n2#from\n28#parse_from\n34\n2#import\n28#parse_import\n34\n2#def\n28#parse_def\n34\n2#class\n28#parse_class\n34\n2#for\n28#parse_for\n34\n2#while\n28#parse_while\n34\n2#if\n28#parse_if\n34\n2#return\n28#parse_return\n34\n2#raise\n28#parse_inner_func\n34\n2#assert\n28#parse_inner_func\n34\n2#break\n28#parse_pass\n34\n2#continue\n28#parse_pass\n34\n2#pass\n28#parse_pass\n34\n2#[\n28#parse_multi_assign\n34\n2#name\n28#parse_assign_or_exp\n34\n2#number\n28#expr\n34\n2#string\n28#expr\n34\n2#try\n28#parse_try\n34\n2#global\n28#parse_global\n34\n2#del\n28#parse_del\n34\n2#;\n28#parse_skip\n34\n2#@\n28#parse_annotation\n34\n26#stmt_map\n"
"63#680\n51#parse_stm\n"
"63#680\n57#256\n"
"63#681\n27\n2#token\n22\n2#type\n22\n25#1\n"
"63#682\n27#1\n28#stmt_map\n17\n10\n37#11\n"
"63#683\n28#parse_error\n27\n27\n2#token\n22\n2#Unknown Expression\n48#3\n30\n35#8\n"
"63#685\n28#stmt_map\n27#1\n22\n27\n48#1\n30\n53\n26#parse_stm\n"
"63#688\n51#parse_block\n"
"63#688\n57#256\n"
"63#689\n28#skip_nl\n27\n48#1\n30\n"
"63#690\n27\n2#token\n22\n2#type\n22\n2#indent\n15\n37#34\n"
"63#691\n27\n2#next\n22\n48\n30\n"
"63#692\n27\n2#token\n22\n2#type\n22\n2#dedent\n16\n37#12\n"
"63#693\n28#parse_stm\n27\n48#1\n30\n"
"63#694\n28#skip_nl\n27\n48#1\n30\n36#18\n"
"63#695\n27\n2#next\n22\n48\n30\n35#49\n27\n2#token\n22\n2#type\n22\n2#eof\n15\n37#4\n24\n52\n35#38\n"
"63#700\n28#parse_stm\n27\n48#1\n30\n"
"63#701\n27\n2#token\n22\n2#type\n22\n2#;\n15\n37#19\n"
"63#702\n27\n2#token\n22\n2#type\n22\n28##7\n17\n37#4\n"
"63#703\n35#8\n35#6\n"
"63#705\n28#parse_stm\n27\n48#1\n30\n36#25\n"
"63#706\n28#skip_nl\n27\n48#1\n30\n53\n26#parse_block\n"
"63#709\n51#parse\n"
"63#709\n57#256\n"
"63#711\n28#tokenize\n27\n48#1\n25#1\n"
"63#712\n28#ParserCtx\n27#1\n27\n48#2\n25#2\n"
"63#713\n27#2\n2#next\n22\n48\n30\n"
"63#714\n47#38\n"
"63#715\n27#2\n2#token\n22\n2#type\n22\n2#eof\n16\n37#7\n"
"63#716\n28#parse_block\n27#2\n48#1\n30\n36#13\n"
"63#717\n27#2\n2#tree\n22\n25#3\n"
"63#719\n27#3\n24\n15\n37#8\n"
"63#720\n27#2\n2#error\n22\n48\n30\n35#1\n"
"63#721\n27#3\n52\n60\n35#20\n46\n25#4\n"
"63#724\n28#compile_error\n2#parse\n27\n27#2\n2#token\n22\n28#str\n27#4\n48#1\n48#4\n30\n"
"63#725\n28#raise\n27#4\n48#1\n30\n53\n26#parse\n"
"63#727\n51#xml_item\n"
"63#727\n57#512\n"
"63#728\n2#<\n27\n4\n2#>\n4\n2#%r\n27#1\n8\n4\n2#</\n4\n27\n4\n2#>\n4\n52\n53\n26#xml_item\n"
"63#730\n51#xml_start\n"
"63#730\n57#256\n"
"63#731\n2#<\n27\n4\n2#>\n4\n52\n53\n26#xml_start\n"
"63#733\n51#xml_close\n"
"63#733\n57#256\n"
"63#734\n2#</\n27\n4\n2#>\n4\n52\n53\n26#xml_close\n"
"63#736\n51#xml_line_head\n"
"63#736\n57#256\n"
"63#737\n2# \n27\n6\n52\n53\n26#xml_line_head\n"
"63#739\n51#print_ast_line_pos\n"
"63#739\n57#256\n"
"63#740\n28#hasattr\n27\n2#pos\n48#2\n37#57\n"
"63#741\n27\n2#pos\n22\n25#1\n"
"63#743\n27#1\n24\n15\n37#4\n24\n52\n35#1\n"
"63#745\n28#len\n27#1\n48#1\n3\n15\n37#4\n24\n52\n35#1\n"
"63#748\n27\n2#pos\n22\n3\n22\n25#2\n"
"63#749\n28#str\n27#2\n48#1\n25#3\n"
"63#750\n3#4.0\n28#len\n27#3\n48#1\n5\n25#4\n"
"63#751\n28#printf\n2#<!--\n27#3\n2#ljust\n22\n3#4.0\n48#1\n4\n2#-->\n4\n48#1\n30\n35#6\n"
"63#753\n28#printf\n2#<!--****-->\n48#1\n30\n53\n26#print_ast_line_pos\n"
"63#755\n51#print_ast_line\n"
"63#755\n57#512\n"
"63#756\n28#print_ast_line_pos\n27#1\n48#1\n30\n"
"63#757\n28#print\n28#xml_line_head\n27\n48#1\n28#xml_item\n27#1\n2#type\n22\n27#1\n2#val\n22\n48#2\n48#2\n30\n53\n26#print_ast_line\n"
"63#759\n51#print_ast_block_start\n"
"63#759\n57#512\n"
"63#760\n28#print_ast_line_pos\n27#1\n48#1\n30\n"
"63#761\n28#print\n28#xml_line_head\n27\n48#1\n28#xml_start\n27#1\n2#type\n22\n48#1\n48#2\n30\n53\n26#print_ast_block_start\n"
"63#763\n51#print_ast_block_close\n"
"63#763\n57#512\n"
"63#764\n28#print_ast_line_pos\n27#1\n48#1\n30\n"
"63#765\n28#print\n28#xml_line_head\n27\n48#1\n28#xml_close\n27#1\n2#type\n22\n48#1\n48#2\n30\n53\n26#print_ast_block_close\n"
"63#768\n51#print_ast_obj\n3\n25#1\n"
"63#768\n57#257\n"
"63#769\n27\n24\n15\n37#4\n24\n52\n35#1\n"
"63#771\n28#gettype\n27\n48#1\n2#list\n15\n37#8\n"
"63#772\n28#print_ast_list\n27\n27#1\n49#2\n52\n35#1\n"
"63#775\n27\n2#type\n22\n28##8\n17\n37#10\n"
"63#776\n28#print_ast_line\n27#1\n27\n48#2\n30\n24\n52\n35#1\n"
"63#780\n27\n2#type\n22\n2#name\n15\n37#8\n"
"63#781\n28#print_ast_line\n27#1\n27\n48#2\n30\n35#7\n"
"63#783\n28#print_ast_block_start\n27#1\n27\n48#2\n30\n"
"63#785\n28#hasattr\n27\n2#first\n48#2\n37#12\n"
"63#786\n28#print_ast\n27\n2#first\n22\n27#1\n3#2.0\n4\n48#2\n30\n35#1\n"
"63#787\n28#hasattr\n27\n2#doc\n48#2\n37#12\n"
"63#788\n28#print_ast\n27\n2#doc\n22\n27#1\n3#2.0\n4\n48#2\n30\n35#1\n"
"63#789\n28#hasattr\n27\n2#second\n48#2\n37#12\n"
"63#790\n28#print_ast\n27\n2#second\n22\n27#1\n3#2.0\n4\n48#2\n30\n35#1\n"
"63#791\n28#hasattr\n27\n2#third\n48#2\n37#12\n"
"63#792\n28#print_ast\n27\n2#third\n22\n27#1\n3#2.0\n4\n48#2\n30\n35#1\n"
"63#794\n28#print_ast_block_close\n27#1\n27\n48#2\n30\n53\n26#print_ast_obj\n"
"63#796\n51#print_ast_list\n3\n25#1\n"
"63#796\n57#257\n"
"63#797\n28#print_ast_line_pos\n27\n48#1\n30\n"
"63#798\n28#print\n28#xml_line_head\n27#1\n48#1\n2#<block>\n48#2\n30\n27\n45\n44#11\n25#2\n"
"63#800\n28#print_ast_obj\n27#2\n27#1\n3#2.0\n4\n48#2\n30\n36#10\n30\n"
"63#801\n28#print_ast_line_pos\n27\n48#1\n30\n"
"63#802\n28#print\n28#xml_line_head\n27#1\n48#1\n2#</block>\n48#2\n30\n53\n26#print_ast_list\n"
"63#804\n51#print_ast\n3\n25#1\n"
"63#804\n57#257\n"
"63#805\n28#print_ast_obj\n27\n27#1\n49#2\n52\n53\n26#print_ast\n"
"63#807\n51#parsefile\n"
"63#807\n57#256\n"
"63#808\n47#10\n"
"63#809\n28#parse\n28#load\n27\n48#1\n49#1\n52\n60\n35#9\n46\n25#1\n"
"63#811\n28#printf\n2#parse file %s FAIL\n27\n48#2\n30\n53\n26#parsefile\n"
"63#813\n51#tk_list_len\n"
"63#813\n57#256\n"
"63#814\n27\n24\n15\n37#5\n"
"63#814\n3\n52\n35#1\n"
"63#815\n27\n2#type\n22\n2#,\n15\n37#15\n"
"63#815\n28#tk_list_len\n27\n2#first\n22\n48#1\n28#tk_list_len\n27\n2#second\n22\n48#1\n4\n52\n35#1\n"
"63#816\n3#1.0\n52\n53\n26#tk_list_len\n"
"63#820\n54#ArgReader\n51#__init__\n"
"63#823\n57#512\n"
"63#824\n27#1\n27\n2#args\n21\n"
"63#825\n3#-1.0\n27\n2#index\n21\n"
"63#826\n28#len\n27#1\n48#1\n3#1.0\n5\n27\n2#max_index\n21\n53\n28#ArgReader\n2#__init__\n21\n51#has_next\n"
"63#828\n57#256\n"
"63#829\n27\n2#index\n22\n27\n2#max_index\n22\n12\n52\n53\n28#ArgReader\n2#has_next\n21\n51#next\n"
"63#831\n57#256\n"
"63#832\n27\n2#index\n22\n3#1.0\n4\n27\n2#index\n21\n"
"63#833\n27\n2#args\n22\n27\n2#index\n22\n22\n52\n53\n28#ArgReader\n2#next\n21\n56\n"
"63#836\n28#__name__\n2#__main__\n15\n37#19\n"
"63#837\n2#sys\n1#1\n"
"63#838\n28#parsefile\n28#sys\n2#argv\n22\n3#1.0\n22\n48#1\n26#tree\n"
"63#839\n28#print_ast\n28#tree\n48#1\n30\n35#1\n61\n";