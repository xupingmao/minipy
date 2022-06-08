/**
 * 调试专用
 * @author xupingmao
 * @since 2022/06/05 23:28:29
 * @modified 2022/06/05 23:29:20
 */

#ifdef RECORD_LAST_OP

CodeQueue* CodeQueue_Init(CodeQueue* queue);
void CodeQueue_Append(CodeQueue* queue, MpCodeCache cache);
MpObj CodeQueue_ToString(CodeQueue* queue);

#endif