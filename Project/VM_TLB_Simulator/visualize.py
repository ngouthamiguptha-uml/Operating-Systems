import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("results.csv")

fig, axes = plt.subplots(2, 2, figsize=(13, 9))
fig.suptitle("VM TLB Simulator Results", fontsize=14, fontweight="bold")

# --- Chart 1: Hit Rate by Policy & Workload (EXP1 only) ---
ax = axes[0, 0]
exp1 = df[df["experiment"] == "EXP1"]
pivot1 = exp1.pivot_table(index="workload", columns="policy", values="hit_rate_pct")
pivot1.plot(kind="bar", ax=ax, color=["#2196F3","#FF9800","#4CAF50"], edgecolor="white", rot=0)
ax.set_title("Policy vs Hit Rate (3 Workloads)")
ax.set_ylabel("Hit Rate (%)"); ax.set_xlabel("")
ax.legend(title="Policy"); ax.grid(axis="y", alpha=0.3)

# --- Chart 2: AMAT by Policy & Workload (EXP1 only) ---
ax = axes[0, 1]
pivot2 = exp1.pivot_table(index="workload", columns="policy", values="amat")
pivot2.plot(kind="bar", ax=ax, color=["#2196F3","#FF9800","#4CAF50"], edgecolor="white", rot=0)
ax.set_title("Policy vs AMAT (3 Workloads)")
ax.set_ylabel("AMAT (cycles)"); ax.set_xlabel("")
ax.legend(title="Policy"); ax.grid(axis="y", alpha=0.3)

# --- Chart 3: Associativity vs Hit Rate (EXP2 only) ---
ax = axes[1, 0]
exp2 = df[df["experiment"] == "EXP2"]
order2 = ["Direct-Mapped", "2-way", "4-way", "8-way", "Fully-Assoc"]
exp2 = exp2.set_index("workload").reindex(order2).reset_index()
ax.plot(exp2["workload"], exp2["hit_rate_pct"], "o-", color="#673AB7", linewidth=2, markersize=8)
for _, row in exp2.iterrows():
    ax.annotate(f"{row['hit_rate_pct']:.1f}%",
                (row["workload"], row["hit_rate_pct"]),
                textcoords="offset points", xytext=(0, 8), ha="center", fontsize=9)
ax.set_title("Associativity vs Hit Rate (LRU, Mixed Conflict)")
ax.set_ylabel("Hit Rate (%)"); ax.tick_params(axis="x", rotation=15)
ax.grid(alpha=0.3)

# --- Chart 4: Context Switch vs Hit Rate (EXP3 only) ---
ax = axes[1, 1]
exp3 = df[df["experiment"] == "EXP3"]
order3 = ["No-Switch","Switch-10000","Switch-5000","Switch-2000","Switch-500"]
exp3 = exp3.set_index("workload").reindex(order3).reset_index()
bars = ax.bar(exp3["workload"], exp3["hit_rate_pct"], color="#E91E63", edgecolor="white")
for bar, val in zip(bars, exp3["hit_rate_pct"]):
    ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
            f"{val:.1f}%", ha="center", va="bottom", fontsize=9)
ax.set_title("Context Switch Frequency vs Hit Rate")
ax.set_ylabel("Hit Rate (%)"); ax.tick_params(axis="x", rotation=20)
ax.grid(axis="y", alpha=0.3)

plt.tight_layout()
plt.savefig("results_charts.png", dpi=130)
print("Charts saved to results_charts.png")
plt.show()
