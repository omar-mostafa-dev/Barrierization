## Results

### Response Time Results

The system demonstrated consistently fast reaction times under different operating scenarios:

- **Congestion activation test**
  - Average response time: **1.41 seconds**

- **Emergency vehicle activation test**
  - Average response time: **0.64 seconds**

These results confirm that the system can respond rapidly, especially in time-critical emergency situations.

---

### System Accuracy Results

System accuracy was evaluated to determine how reliably the detection model
identified vehicles and provided correct congestion information.

The following performance metrics were obtained:
- **Precision**: 0.9958
- **Recall**: 0.9883
- **mAP@50**: 0.9946
- **mAP@50–95**: 0.9276

The obtained metrics indicate:
- High vehicle detection reliability
- Accurate bounding-box detection
- Strong overall model performance suitable for real-time traffic analysis

---

### Congestion Percentage Results

The road initially operated with **3 lanes**, supporting a maximum capacity of
**9 vehicles**.

- When congestion exceeded **60%**, the emergency lane was activated,
  increasing the road capacity to **4 lanes (12 vehicles)**.

After activation:
- Congestion percentage decreased from **66.7% to 50%**
- Road efficiency improved without compromising safety

These results confirm that the Intelligent Road system effectively reduces
traffic congestion and fulfills the **Congestion Reduction design requirement**.
