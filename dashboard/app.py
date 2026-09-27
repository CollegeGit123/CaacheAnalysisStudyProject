import streamlit as st
import pandas as pd
import plotly.express as px

st.set_page_config(
    page_title="CacheLab Dashboard",
    page_icon="📊",
    layout="wide"
)

st.title("CacheLab — Cache Replacement Analysis")
st.caption("Comparative analysis of FIFO, LRU, LFU, ARC and TinyLFU")

# Project & Experiment Overview
st.subheader("Project & Experiment Overview")

overview1, overview2, overview3, overview4, overview5 = st.columns(5)

overview1.metric("Cache Policies", "5")
overview2.metric("Workloads", "4")
overview3.metric("Cache Capacities", "5")
overview4.metric("Operations / Trace", "100,000")
overview5.metric("Total Experiments", "100")

st.markdown(
    """
    **Policies:** FIFO · LRU · LFU · ARC · TinyLFU

    **Workloads:** Uniform Random · Zipfian · Cyclic Scan · Dynamic Phase-Shift

    **Metrics:** Hit Rate · Miss Rate · Evictions · Execution Time
    """
)

st.divider()
# Load benchmark results
@st.cache_data
def load_data():
    return pd.read_csv("results.csv")

df = load_data()

# Sidebar filters
st.sidebar.header("Filters")

workloads = st.sidebar.multiselect(
    "Workload",
    sorted(df["Workload"].unique()),
    default=sorted(df["Workload"].unique())
)

policies = st.sidebar.multiselect(
    "Policy",
    sorted(df["Policy"].unique()),
    default=sorted(df["Policy"].unique())
)

capacities = st.sidebar.multiselect(
    "Cache Capacity",
    sorted(df["Capacity"].unique()),
    default=sorted(df["Capacity"].unique())
)

filtered = df[
    df["Workload"].isin(workloads)
    & df["Policy"].isin(policies)
    & df["Capacity"].isin(capacities)
]

# ============================================================
# ============================================================
# CUSTOM SIMULATION
# ============================================================

st.divider()
st.subheader("Custom Simulation")

st.write(
    "Enter a cache policy, capacity, and request trace to simulate "
    "cache behavior interactively."
)

custom_col1, custom_col2 = st.columns(2)

with custom_col1:
    custom_policy = st.selectbox(
        "Cache Policy",
        ["FIFO", "LRU", "LFU", "ARC", "TinyLFU"],
        key="custom_policy"
    )

with custom_col2:
    custom_capacity = st.number_input(
        "Cache Capacity",
        min_value=1,
        max_value=10000,
        value=3,
        step=1,
        key="custom_capacity"
    )

custom_trace_input = st.text_area(
    "Request Trace",
    placeholder="Example: A B C A D A E",
    height=100,
    key="custom_trace"
)

run_custom = st.button(
    "Run Simulation",
    type="primary",
    use_container_width=True
)

if run_custom:

    if not custom_trace_input.strip():
        st.warning("Enter at least one request in the trace.")

    else:
        custom_trace = custom_trace_input.replace(",", " ").split()
        st.caption(f"Trace length: {len(custom_trace)} requests")

        cache = []
        hits = 0
        misses = 0
        evictions = 0
        steps = []

        access_order = []
        frequencies = {}
        request_frequency = {}

        for step, key in enumerate(custom_trace, start=1):

            request_frequency[key] = request_frequency.get(key, 0) + 1

            hit = key in cache
            evicted = ""

            if hit:
                hits += 1

            else:
                misses += 1

                if len(cache) < custom_capacity:
                    cache.append(key)

                else:

                    if custom_policy == "FIFO":
                        victim = cache.pop(0)
                        cache.append(key)

                        evicted = victim
                        evictions += 1

                    elif custom_policy == "LRU":
                        victim = access_order[0]
                        access_order.remove(victim)
                        cache.remove(victim)
                        cache.append(key)

                        evicted = victim
                        evictions += 1

                    elif custom_policy == "LFU":
                        victim = min(
                            cache,
                            key=lambda x: (
                                frequencies.get(x, 0),
                                cache.index(x)
                            )
                        )

                        cache.remove(victim)
                        cache.append(key)

                        evicted = victim
                        evictions += 1

                    elif custom_policy == "ARC":
                        # Simplified interactive visualization.
                        # The C++ ARC implementation is the
                        # authoritative implementation for benchmarks.
                        victim = cache.pop(0)
                        cache.append(key)

                        evicted = victim
                        evictions += 1

                    else:  # TinyLFU
                        # Simplified admission visualization.
                        # The C++ TinyLFU implementation is the
                        # authoritative implementation for benchmarks.
                        candidate_frequency = request_frequency.get(key, 0)

                        victim = min(
                            cache,
                            key=lambda x: (
                                request_frequency.get(x, 0),
                                cache.index(x)
                            )
                        )

                        victim_frequency = request_frequency.get(victim, 0)

                        if candidate_frequency > victim_frequency:
                            cache.remove(victim)
                            cache.append(key)

                            evicted = victim
                            evictions += 1

            if custom_policy == "LRU":

                if key in access_order:
                    access_order.remove(key)

                if key in cache:
                    access_order.append(key)

            elif custom_policy == "LFU":

                frequencies[key] = frequencies.get(key, 0) + 1

            steps.append(
                {
                    "Step": step,
                    "Request": key,
                    "Result": "HIT" if hit else "MISS",
                    "Evicted": evicted if evicted else "-",
                    "Cache State": "  ".join(cache)
                }
            )

        total_operations = len(custom_trace)

        hit_rate = (
            hits / total_operations * 100
            if total_operations
            else 0
        )

        miss_rate = (
            misses / total_operations * 100
            if total_operations
            else 0
        )

        st.markdown("### Simulation Results")

        result1, result2, result3, result4 = st.columns(4)

        result1.metric("Hits", hits)
        result2.metric("Misses", misses)
        result3.metric("Hit Rate", f"{hit_rate:.2f}%")
        result4.metric("Evictions", evictions)

        st.markdown("### Step-by-Step Execution")

        step_data = [
            {
                "Step": row["Step"],
                "Request": row["Request"],
                "Result": row["Result"],
                "Evicted": row["Evicted"] if row["Evicted"] else "-",
                "Cache State": row["Cache State"]
            }
            for row in steps
        ]

        st.dataframe(
            step_data,
            use_container_width=True,
            hide_index=True,
            column_config={
                "Step": st.column_config.NumberColumn("Step", width="small"),
                "Request": st.column_config.TextColumn("Request"),
                "Result": st.column_config.TextColumn("Result"),
                "Evicted": st.column_config.TextColumn("Evicted"),
                "Cache State": st.column_config.TextColumn("Cache State")
            }
        )

        st.markdown("### Final Cache State")

        if cache:
            st.code("  ".join(cache))
        else:
            st.info("Cache is empty.")

# Summary metrics
st.subheader("Benchmark Summary")

col1, col2, col3, col4 = st.columns(4)

col1.metric(
    "Experiments",
    len(filtered)
)

col2.metric(
    "Average Hit Rate",
    f"{filtered['HitRate'].mean():.2f}%"
)

col3.metric(
    "Average Miss Rate",
    f"{filtered['MissRate'].mean():.2f}%"
)

col4.metric(
    "Avg Execution Time",
    f"{filtered['ExecutionTimeMs'].mean():.2f} ms"
)

st.divider()

# Hit rate comparison
st.subheader("Hit Rate by Policy")

fig = px.bar(
    filtered,
    x="Policy",
    y="HitRate",
    color="Workload",
    facet_col="Capacity",
    barmode="group",
    labels={
        "HitRate": "Hit Rate (%)",
        "Policy": "Cache Policy"
    }
)

fig.update_layout(
    height=550,
    legend_title="Workload"
)

st.plotly_chart(fig, use_container_width=True)

# Hit rate vs capacity
st.subheader("Hit Rate vs Cache Capacity")

capacity_data = (
    filtered
    .groupby(["Policy", "Workload", "Capacity"], as_index=False)
    ["HitRate"]
    .mean()
)

fig = px.line(
    capacity_data,
    x="Capacity",
    y="HitRate",
    color="Policy",
    facet_col="Workload",
    markers=True,
    labels={
        "HitRate": "Hit Rate (%)",
        "Capacity": "Cache Capacity"
    }
)

fig.update_layout(height=500)

st.plotly_chart(fig, use_container_width=True)

# Workload × Policy heatmap
st.subheader("Workload × Policy Hit Rate")

heatmap_data = (
    filtered
    .groupby(["Workload", "Policy"])["HitRate"]
    .mean()
    .reset_index()
    .pivot(
        index="Workload",
        columns="Policy",
        values="HitRate"
    )
)

fig = px.imshow(
    heatmap_data,
    text_auto=".2f",
    aspect="auto",
    labels={
        "x": "Cache Policy",
        "y": "Workload",
        "color": "Average Hit Rate (%)"
    }
)

fig.update_layout(
    height=450
)

st.plotly_chart(fig, use_container_width=True)

# Execution time
st.subheader("Execution Time")

fig = px.bar(
    filtered,
    x="Policy",
    y="ExecutionTimeMs",
    color="Workload",
    facet_col="Capacity",
    barmode="group",
    labels={
        "ExecutionTimeMs": "Execution Time (ms)"
    }
)

fig.update_layout(height=500)

st.plotly_chart(fig, use_container_width=True)

# Evictions
st.subheader("Eviction Count")

fig = px.bar(
    filtered,
    x="Policy",
    y="Evictions",
    color="Workload",
    facet_col="Capacity",
    barmode="group",
    labels={
        "Evictions": "Evictions"
    }
)

fig.update_layout(height=500)

st.plotly_chart(fig, use_container_width=True)

# Detailed results
st.subheader("Detailed Benchmark Results")

display_columns = [
    "Policy",
    "Workload",
    "Capacity",
    "Operations",
    "Hits",
    "Misses",
    "Evictions",
    "HitRate",
    "MissRate",
    "ExecutionTimeMs"
]

st.dataframe(
    filtered[display_columns],
    use_container_width=True,
    hide_index=True
)
st.divider()

st.subheader("Algorithm Overview")

algorithm_data = {
    "FIFO": {
        "Strategy": "Evict the oldest inserted entry",
        "Data Structure": "Queue",
        "Expected Access": "O(1)"
    },
    "LRU": {
        "Strategy": "Evict the least recently accessed entry",
        "Data Structure": "Hash Map + Doubly Linked List",
        "Expected Access": "O(1)"
    },
    "LFU": {
        "Strategy": "Evict the least frequently accessed entry",
        "Data Structure": "Hash Map + Frequency Lists",
        "Expected Access": "O(1) expected"
    },
    "ARC": {
        "Strategy": "Adapt between recent and frequent access patterns",
        "Data Structure": "T1/T2 + B1/B2 Ghost Lists",
        "Expected Access": "O(1) expected"
    },
    "TinyLFU": {
        "Strategy": "Frequency-based admission with approximate frequency tracking",
        "Data Structure": "Count-Min Sketch + Cache",
        "Expected Access": "O(1) expected"
    }
}

for policy, details in algorithm_data.items():
    with st.expander(policy):
        col1, col2, col3 = st.columns(3)

        col1.write("**Strategy**")
        col1.write(details["Strategy"])

        col2.write("**Data Structure**")
        col2.write(details["Data Structure"])

        col3.write("**Expected Complexity**")
        col3.write(details["Expected Access"])

        st.divider()

st.subheader("Findings & Observations")

if not filtered.empty:

    avg_policy = (
        filtered.groupby("Policy")["HitRate"]
        .mean()
        .sort_values(ascending=False)
    )

    avg_workload = (
        filtered.groupby("Workload")["HitRate"]
        .mean()
        .sort_values(ascending=False)
    )

    highest_policy = avg_policy.index[0]
    highest_policy_rate = avg_policy.iloc[0]

    lowest_policy = avg_policy.index[-1]
    lowest_policy_rate = avg_policy.iloc[-1]

    best_workload = avg_workload.index[0]
    best_workload_rate = avg_workload.iloc[0]

    lowest_workload = avg_workload.index[-1]
    lowest_workload_rate = avg_workload.iloc[-1]

    col1, col2 = st.columns(2)

    with col1:
        st.markdown("### Policy-level observation")
        st.write(
            f"Across the currently selected experiments, "
            f"**{highest_policy}** has the highest average hit rate "
            f"({highest_policy_rate:.2f}%), while "
            f"**{lowest_policy}** has the lowest average hit rate "
            f"({lowest_policy_rate:.2f}%)."
        )

    with col2:
        st.markdown("### Workload-level observation")
        st.write(
            f"The highest average hit rate occurs for "
            f"**{best_workload}** ({best_workload_rate:.2f}%), while "
            f"the lowest occurs for "
            f"**{lowest_workload}** ({lowest_workload_rate:.2f}%)."
        )

    st.markdown("### Capacity observation")

    capacity_summary = (
        filtered.groupby("Capacity")["HitRate"]
        .mean()
        .sort_index()
    )

    if len(capacity_summary) >= 2:
        smallest_capacity = capacity_summary.index[0]
        largest_capacity = capacity_summary.index[-1]

        smallest_rate = capacity_summary.iloc[0]
        largest_rate = capacity_summary.iloc[-1]

        change = largest_rate - smallest_rate

        st.write(
            f"Average hit rate changes from **{smallest_rate:.2f}%** "
            f"at capacity {smallest_capacity} to **{largest_rate:.2f}%** "
            f"at capacity {largest_capacity}, a change of "
            f"**{change:+.2f} percentage points**."
        )

else:
    st.info("Select at least one workload, policy, and capacity to generate observations.")