#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// 1. ثوابت الحماية والحركة (Safety & Motion)
// ==========================================
const int MAX_PWM = 230;     // أقصى سرعة آمنة للدرايفر
const int STEP_SIZE = 5;     // مقدار التسارع في الـ Soft Start
const int LOOP_DELAY = 10;   // سرعة دورة المعالج بالملي ثانية

// ==========================================
// 2. توصيلات الدرايفر الأول (TB6612FNG #1 - الجانب الأيسر مثلاً)
// ==========================================
#define MOT1_PWM_PIN  IO4    // Pin الـ PWM
#define MOT1_IN1_PIN  IO17    // اتجاه 1
#define MOT1_IN2_PIN  IO16   // اتجاه 2

#define MOT2_PWM_PIN  IO21
#define MOT2_IN1_PIN  IO18
#define MOT2_IN2_PIN  IO19

// ==========================================
// 3. توصيلات الدرايفر الثاني (TB6612FNG #2 - الجانب الأيمن)
// ==========================================
#define MOT3_PWM_PIN  IO32
#define MOT3_IN1_PIN  IO25
#define MOT3_IN2_PIN  IO33

#define MOT4_PWM_PIN  IO14
#define MOT4_IN1_PIN  IO26
#define MOT4_IN2_PIN  IO27


#endif // CONFIG_H