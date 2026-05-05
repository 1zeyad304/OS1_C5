
const COLORS = [
    "#2563eb", "#7c3aed", "#059669", "#dc2626",
    "#d97706", "#0891b2", "#be185d", "#65a30d",
    "#ea580c", "#6366f1", "#14b8a6", "#f43f5e"
];
 
/* ── Generate input table ── */
function generateTable() {
    const n = parseInt(document.getElementById("num-processes").value);
 
    if (isNaN(n) || n <= 0 || n > 20) {
        alert("Please enter a number between 1 and 20.");
        return;
    }
 
    const tbody = document.getElementById("input-body");
    tbody.innerHTML = "";
 
    for (let i = 0; i < n; i++) {
        const row = document.createElement("tr");
        row.innerHTML = `
            <td><input type="number" id="pid-${i}" value="${i + 1}" min="1"></td>
            <td><input type="number" id="at-${i}"  value="0"        min="0"></td>
            <td><input type="number" id="bt-${i}"  value=""         min="1" placeholder="required"></td>
        `;
        tbody.appendChild(row);
    }
 
    document.getElementById("process-table-wrapper").style.display = "block";
    document.getElementById("results-section").style.display       = "none";
    document.getElementById("error-msg").textContent               = "";
}
 
/* ── Client-side validation ── */
function validateInput(processes, quantum) {
    if (isNaN(quantum) || quantum <= 0)
        return "Time Quantum must be a positive integer.";
 
    const pids = [];
    for (let i = 0; i < processes.length; i++) {
        const p = processes[i];
 
        if (isNaN(p.pid) || p.pid <= 0)
            return `Process ${i + 1}: Invalid PID.`;
 
        if (isNaN(p.at) || p.at < 0)
            return `Process ${i + 1}: Arrival Time must be >= 0.`;
 
        if (isNaN(p.bt) || p.bt <= 0)
            return `Process ${i + 1}: Burst Time must be > 0.`;
 
        if (pids.includes(p.pid))
            return `Duplicate PID found: ${p.pid}`;
 
        pids.push(p.pid);
    }
    return null;
}
 
/* ── Run simulation ── */
async function runSimulation() {
    const n       = parseInt(document.getElementById("num-processes").value);
    const quantum = parseInt(document.getElementById("quantum").value);
    const errorDiv = document.getElementById("error-msg");
    errorDiv.textContent = "";
 
    const processes = [];
    for (let i = 0; i < n; i++) {
        processes.push({
            pid: parseInt(document.getElementById(`pid-${i}`).value),
            at:  parseInt(document.getElementById(`at-${i}`).value),
            bt:  parseInt(document.getElementById(`bt-${i}`).value),
        });
    }
 
    const err = validateInput(processes, quantum);
    if (err) {
        errorDiv.textContent = "❌ " + err;
        return;
    }
 
    /* Build input string for simulator:  n\nquantum\npid at bt\n... */
    let input = `${n}\n${quantum}\n`;
    processes.forEach(p => {
        input += `${p.pid} ${p.at} ${p.bt}\n`;
    });
 
    try {
        const response = await fetch("/run", {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: input
        });
 
        if (!response.ok) throw new Error("Server error");
 
        const data = await response.json();
 
        if (data.error) {
            errorDiv.textContent = "❌ " + data.error;
            return;
        }
 
        displayResults(data, processes, quantum);
 
    } catch (e) {
        errorDiv.textContent = "❌ Could not connect to server. Make sure the backend is running.";
    }
}
 
/* ── Display all results ── */
function displayResults(data, processes, quantum) {
    document.getElementById("results-section").style.display = "block";
    document.getElementById("quantum-display").textContent   = quantum;
 
    renderReadyQueue(data.rr.gantt, processes, quantum);
    renderGantt("gantt-rr",   data.rr.gantt);
    renderGantt("gantt-sjf",  data.sjf.gantt);
    renderGantt("gantt-srtf", data.srtf.gantt);
    renderResultsTable("table-rr",   data.rr);
    renderResultsTable("table-sjf",  data.sjf);
    renderResultsTable("table-srtf", data.srtf);
    renderComparison(data.rr, data.sjf, data.srtf);
    renderConclusion(data.rr, data.sjf, data.srtf, quantum);
 
    document.getElementById("results-section").scrollIntoView({ behavior: "smooth" });
}
 
/* ── Ready Queue Visualisation ── */
function renderReadyQueue(gantt, processes, quantum) {
    const container = document.getElementById("ready-queue-view");
    container.innerHTML = "";
 
    /* Show first few time slices as a visual queue snapshot */
    const maxSlots = Math.min(gantt.length, 12);
 
    for (let i = 0; i < maxSlots; i++) {
        const block   = gantt[i];
        const slot    = document.createElement("div");
        slot.className = "rq-slot";
 
        const b       = document.createElement("div");
        b.className   = "rq-block";
        b.style.background = COLORS[(block.pid - 1) % COLORS.length];
        b.textContent = `P${block.pid}`;
 
        const lbl     = document.createElement("div");
        lbl.className = "rq-label";
        lbl.textContent = `t=${block.start}`;
 
        slot.appendChild(b);
        slot.appendChild(lbl);
        container.appendChild(slot);
 
        if (i < maxSlots - 1) {
            const arrow       = document.createElement("div");
            arrow.className   = "rq-arrow";
            arrow.textContent = "→";
            container.appendChild(arrow);
        }
    }
 
    if (gantt.length > 12) {
        const more       = document.createElement("div");
        more.className   = "rq-arrow";
        more.textContent = `… +${gantt.length - 12} more slices`;
        container.appendChild(more);
    }
}
 
/* ── Gantt Chart ── */
function renderGantt(containerId, gantt) {
    const container  = document.getElementById(containerId);
    container.innerHTML = "";
 
    const wrapper    = document.createElement("div");
    wrapper.className = "gantt-wrapper";
 
    const blocksDiv  = document.createElement("div");
    blocksDiv.className = "gantt-blocks";
 
    const timesDiv   = document.createElement("div");
    timesDiv.className = "gantt-times";
 
    const totalTime  = gantt[gantt.length - 1].end - gantt[0].start;
    const minWidth   = 54;
 
    gantt.forEach((block, i) => {
        const duration = block.end - block.start;
        const width    = Math.max(minWidth, (duration / totalTime) * 700);
 
        const div       = document.createElement("div");
        div.className   = "gantt-block";
        div.style.width = width + "px";
        div.style.background = COLORS[(block.pid - 1) % COLORS.length];
        div.textContent = `P${block.pid}`;
        div.title       = `P${block.pid} | Start: ${block.start} | End: ${block.end} | Duration: ${duration}`;
        blocksDiv.appendChild(div);
 
        const timeLabel       = document.createElement("div");
        timeLabel.className   = "gantt-time-label";
        timeLabel.style.width = width + "px";
        timeLabel.textContent = block.start;
        timesDiv.appendChild(timeLabel);
 
        if (i === gantt.length - 1) {
            const lastLabel       = document.createElement("div");
            lastLabel.className   = "gantt-time-label";
            lastLabel.textContent = block.end;
            timesDiv.appendChild(lastLabel);
        }
    });
 
    wrapper.appendChild(blocksDiv);
    wrapper.appendChild(timesDiv);
    container.appendChild(wrapper);
}
 
/* ── Results Table ── */
function renderResultsTable(containerId, data) {
    const container = document.getElementById(containerId);
 
    let html = `
        <table>
            <thead>
                <tr>
                    <th>PID</th>
                    <th>Arrival Time</th>
                    <th>Burst Time</th>
                    <th>Waiting Time</th>
                    <th>Turnaround Time</th>
                    <th>Response Time</th>
                </tr>
            </thead>
            <tbody>
    `;
 
    data.processes.forEach(p => {
        html += `
            <tr>
                <td>P${p.pid}</td>
                <td>${p.at}</td>
                <td>${p.bt}</td>
                <td>${p.wt}</td>
                <td>${p.tat}</td>
                <td>${p.rt}</td>
            </tr>
        `;
    });
 
    html += `
            <tr class="avg-row">
                <td colspan="3">Averages</td>
                <td>${data.avg_wt}</td>
                <td>${data.avg_tat}</td>
                <td>${data.avg_rt}</td>
            </tr>
            </tbody>
        </table>
    `;
 
    container.innerHTML = html;
}
 
/* ── Comparison Summary ── */
function renderComparison(rr, sjf, srtf) {
    const container = document.getElementById("comparison");

    const metrics = [
        { label: "Avg Waiting Time",    rVal: rr.avg_wt,  sVal: sjf.avg_wt,  stVal: srtf.avg_wt  },
        { label: "Avg Turnaround Time", rVal: rr.avg_tat, sVal: sjf.avg_tat, stVal: srtf.avg_tat },
        { label: "Avg Response Time",   rVal: rr.avg_rt,  sVal: sjf.avg_rt,  stVal: srtf.avg_rt  },
    ];

    let html = `<div class="comparison-grid" style="grid-template-columns: 2fr 1fr 1fr 1fr;">
        <div class="comp-header">Metric</div>
        <div class="comp-header">Round Robin</div>
        <div class="comp-header">SJF (Non-Preemptive)</div>
        <div class="comp-header">SRTF (Preemptive)</div>
    `;

    metrics.forEach(m => {
        const vals = [parseFloat(m.rVal), parseFloat(m.sVal), parseFloat(m.stVal)];
        const min  = Math.min(...vals);
        html += `<div class="comp-cell"><strong>${m.label}</strong></div>`;
        html += `<div class="${vals[0] === min ? "winner" : "loser"}">${m.rVal} ${vals[0] === min ? "🏆" : ""}</div>`;
        html += `<div class="${vals[1] === min ? "winner" : "loser"}">${m.sVal} ${vals[1] === min ? "🏆" : ""}</div>`;
        html += `<div class="${vals[2] === min ? "winner" : "loser"}">${m.stVal} ${vals[2] === min ? "🏆" : ""}</div>`;
    });

    html += `</div>`;
    container.innerHTML = html;
}

/* ── Final Conclusion ── */
function renderConclusion(rr, sjf, srtf, quantum) {
    const container = document.getElementById("conclusion");

    const allVals = {
        wt:  { rr: parseFloat(rr.avg_wt),  sjf: parseFloat(sjf.avg_wt),  srtf: parseFloat(srtf.avg_wt)  },
        tat: { rr: parseFloat(rr.avg_tat), sjf: parseFloat(sjf.avg_tat), srtf: parseFloat(srtf.avg_tat) },
        rt:  { rr: parseFloat(rr.avg_rt),  sjf: parseFloat(sjf.avg_rt),  srtf: parseFloat(srtf.avg_rt)  },
    };

    function winner(obj) {
        const min = Math.min(obj.rr, obj.sjf, obj.srtf);
        if (obj.rr === min)   return "Round Robin";
        if (obj.srtf === min) return "SRTF (Preemptive SJF)";
        return "SJF (Non-Preemptive)";
    }

    const wtW  = winner(allVals.wt);
    const tatW = winner(allVals.tat);
    const rtW  = winner(allVals.rt);

    container.innerHTML = `
        <div class="conclusion-box">
            <p>📌 <strong>Waiting Time:</strong>
               ${wtW} performed best
               (RR: ${rr.avg_wt} | SJF: ${sjf.avg_wt} | SRTF: ${srtf.avg_wt})</p>

            <p>📌 <strong>Turnaround Time:</strong>
               ${tatW} performed best
               (RR: ${rr.avg_tat} | SJF: ${sjf.avg_tat} | SRTF: ${srtf.avg_tat})</p>

            <p>📌 <strong>Response Time:</strong>
               ${rtW} performed best
               (RR: ${rr.avg_rt} | SJF: ${sjf.avg_rt} | SRTF: ${srtf.avg_rt})</p>

            <p>🔄 <strong>Quantum Effect:</strong>
               With quantum = <strong>${quantum}</strong>, Round Robin preempts every
               ${quantum} time unit(s). A smaller quantum improves response time but
               increases context-switch overhead; a larger quantum makes RR behave
               more like FCFS.</p>

            <p>⚖️ <strong>Main Trade-off:</strong>
               Round Robin is fair — every process gets CPU time regularly.
               SJF (Non-Preemptive) minimises average waiting time but cannot
               interrupt a running process. SRTF (Preemptive SJF) is the most
               efficient on average metrics but may starve long processes if
               short jobs keep arriving.</p>

            <p>🏆 <strong>Overall Recommendation:</strong>
               For throughput and efficiency: <strong>SRTF</strong>.
               For fairness and interactive systems: <strong>Round Robin</strong>.
               For simple batch systems: <strong>SJF (Non-Preemptive)</strong>.</p>

            <p>🤝 <strong>Fairness:</strong>
               Round Robin distributes CPU time most evenly. SRTF and SJF may
               cause long processes to wait significantly when short jobs are present.</p>
        </div>
    `;
}