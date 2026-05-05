# ⚙️ CPU Scheduling Simulator — C5
## Round Robin vs Shortest Job First (SJF)

**Operating Systems 1 — Algorithm Comparison Project**

---

## 👥 Team Members

| # | Name | Student ID |
|---|------|------------|
| 1 | Ziad Mohamed Mohamed El-Rawi | 20240389 |
| 2 | Khaled Ahmed Hamdy | 20240321 |
| 3 | Ziad Amro Gamal | 20240382 |
| 4 | Shady Abdo Mohamed | 20240471 |
| 5 | Ali Mohamed Abd El-Salam | 20240603 |
| 6 | Omar Mohamed Osama | 20240638 |

---

## 🚀 How to Run

**1.** نزّل Python من 👉 [python.org](https://www.python.org/downloads/)
> أثناء التثبيت تأكد تعلّم ✅ **Add Python to PATH**

**2.** دبل كليك على `run.bat` ✅

بس كده! المتصفح هيفتح تلقائياً على:
```
http://localhost:8080
```

---

## 📋 Project Description

يقارن المشروع ده بين خوارزميتين لجدولة الـ CPU:

- **Round Robin (RR)** — بتدي كل process وقت متساوي (time quantum) بالتساوي
- **Shortest Job First (SJF)** — بتشغّل الـ process الأقصر burst time الأول، وموجودة بنسختين:
  - **Non-Preemptive** — لما الـ process تبدأ بتكمل لآخرها
  - **Preemptive (SRTF)** — لو جت process جديدة أقصر من اللي بتشتغل، بتوقفها وتبدأ الأقصر

الهدف هو المقارنة بين العدالة في توزيع الـ CPU (RR) مقابل الكفاءة في إنهاء الـ jobs الصغيرة (SJF).

---

## 🛠️ Implementation Technology

- **Scheduling Logic:** C (compiled to `simulator.exe`)
- **Backend Server:** Python 3
- **GUI & Visualization:** HTML + CSS + JavaScript (Browser-based)

---

## 📁 Project Structure

```
OS_C5/
├── src/
│   ├── process.h         → Process struct
│   ├── main.c            → Entry point
│   ├── validation.h/c    → Input validation
│   ├── rr.h/c            → Round Robin algorithm
│   ├── sjf.h/c           → SJF Non-Preemptive algorithm
│   ├── srtf.h/c          → SJF Preemptive (SRTF) algorithm
│   └── metrics.h/c       → WT, TAT, RT calculations
├── screenshots/          → Test scenario screenshots
├── test-cases/           → Sample test case inputs
├── simulator.exe         → Pre-built C simulator
├── server.py             → Python HTTP backend
├── index.html            → Main UI page
├── style.css             → Styling
├── app.js                → Frontend logic
├── run.bat               → One-click launcher
└── README.md             → This file
```

---

## 🖥️ Interface Sections

- **Input Panel** — عدد الـ processes والـ time quantum
- **Ready Queue View** — عرض حركة الـ queue في RR
- **Gantt Chart (RR)** — timeline تنفيذ Round Robin
- **Gantt Chart (SJF Non-Preemptive)** — timeline تنفيذ SJF العادي
- **Gantt Chart (SJF Preemptive / SRTF)** — timeline تنفيذ SJF مع الـ preemption
- **Results Table (RR)** — WT, TAT, RT لكل process + المتوسطات
- **Results Table (SJF)** — WT, TAT, RT لكل process + المتوسطات
- **Comparison Summary** — مقارنة جنب لجنب مع الفايز
- **Final Conclusion** — تحليل تلقائي وتوصية

---

## ✅ Input Validation

بيرفض أي input غلط:
- Arrival time سالب
- Burst time صفر أو سالب
- Process ID متكرر
- Quantum مش صح أو ناقص
- حروف في خانات الأرقام

---

## 🧪 Test Scenarios

| Scenario | Description |
|----------|-------------|
| A — Basic mixed workload | processes مختلفة في الـ arrival والـ burst |
| B — Short-job-heavy | كتير من الـ jobs الصغيرة عشان SJF يظهر واضح |
| C — Fairness case | يوضح إن RR بيوزع الـ CPU بشكل أعدل |
| D — Long-job sensitivity | process طويلة بتتنافس مع قصيرة |
| E — Validation case | input غلط عشان يتعرض الـ validation |

---

## 📊 Metrics Calculated

| Metric | Formula |
|--------|---------|
| Turnaround Time (TAT) | Completion Time − Arrival Time |
| Waiting Time (WT) | TAT − Burst Time |
| Response Time (RT) | First Run Time − Arrival Time |

---

## ⚠️ Project-Specific Notes

- كلا الخوارزميتين بيشتغلوا على **نفس الـ workload** دايماً
- SJF موجود بنسختين: **Non-Preemptive** و **Preemptive (SRTF)**
  - في الـ **Non-Preemptive**: لما الـ process تبدأ بتكمل لآخرها
  - في الـ **Preemptive (SRTF)**: لو جت process جديدة remaining time بتاعتها أقل من اللي شغالة، الـ CPU بتوقفها وتبدأ الأقصر
- الـ time quantum في RR بيحدده المستخدم وقت التشغيل
- Tie-breaking في SJF: لو اتنين نفس الـ burst → الأقدم في الـ arrival يفوز
- المشروع بيناقش العدالة مقابل الكفاءة وتأثير الـ quantum على النتايج
