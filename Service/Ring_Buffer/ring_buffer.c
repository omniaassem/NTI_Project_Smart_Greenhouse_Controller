#include "ring_buffer.h"

/**
 * @brief تهيئة الـ Ring Buffer وتصفير المؤشرات
 * @param buf مؤشر لهيكل الـ Ring Buffer
 */
void RingBuffer_Init(RingBuffer_t *buf) {
    if (buf != NULL) {
        buf->head = 0;
        buf->tail = 0;
    }
}

/**
 * @brief إضافة عنصر جديد داخل الـ Ring Buffer
 * @param buf مؤشر للهيكل
 * @param data Byte القراءة المراد إدخاله
 * @return STD_ReturnType E_OK عند النجاح، E_NOK عند امتلاء البافر أو المؤشر الفارغ
 */
STD_ReturnType RingBuffer_Put(RingBuffer_t *buf, u8 data) {
    if (buf == NULL) return E_NOK;

    u8 next = (u8)((buf->head + 1u) % RING_BUFFER_SIZE);
    
    /* التحقق من امتلاء الـ Buffer */
    if (next == buf->tail) {
        return E_NOK; // Buffer Full
    }

    buf->buffer[buf->head] = data;
    buf->head = next;
    return E_OK;
}

/**
 * @brief سحب عنصر من الـ Ring Buffer
 * @param buf مؤشر للهيكل
 * @param data مؤشر لتخزين القيمة المسحوبة
 * @return STD_ReturnType E_OK عند النجاح، E_NOK عند فراغ البافر أو المؤشر الفارغ
 */
STD_ReturnType RingBuffer_Get(RingBuffer_t *buf, u8 *data) {
    if ((buf == NULL) || (data == NULL)) return E_NOK;

    /* التحقق من أن الـ Buffer ليس فارغاً */
    if (buf->head == buf->tail) {
        return E_NOK; // Buffer Empty
    }

    *data = buf->buffer[buf->tail];
    buf->tail = (u8)((buf->tail + 1u) % RING_BUFFER_SIZE);
    return E_OK;
}

/**
 * @brief التحقق مما إذا كان الـ Ring Buffer فارغاً
 * @param buf مؤشر للهيكل
 * @return u8 1 إذا كان فارغاً، 0 إذا كان يحتوي على بيانات
 */
u8 RingBuffer_IsEmpty(const RingBuffer_t *buf) {
    if (buf == NULL) return 1u;
    return (buf->head == buf->tail) ? 1u : 0u;
}
