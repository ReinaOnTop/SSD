<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Informatika SSD — Collection Tracker</title
  <style>
    :root{
      --bg:#0e0f14; --panel:#171821; --muted:#9aa0b4; --accent:#81689D; --accent-2:#FFD0EC; --cell:#121219;
      --radius:12px; --gap:12px; font-family:Inter, ui-sans-serif, system-ui, -apple-system, 'Segoe UI', Roboto, 'Helvetica Neue', Arial;
    }
    html,body{height:100%;margin:0;background:linear-gradient(180deg,#0b0c10 0%, #0e0f14 100%);color:#e6e7ee}
    .wrap{max-width:1200px;margin:28px auto;padding:20px;gap:16px;display:flex;flex-direction:column}
    header{display:flex;align-items:center;justify-content:space-between}
    h1{font-size:20px;margin:0;color:var(--accent)}
    .controls{display:flex;gap:8px;align-items:center}
    button{background:linear-gradient(180deg, rgba(255,255,255,0.02), rgba(0,0,0,0.06));border:1px solid rgba(255,255,255,0.03);color:inherit;padding:8px 12px;border-radius:10px;cursor:pointer}
    button.primary{background:linear-gradient(180deg, rgba(129,104,157,0.12), rgba(129,104,157,0.06));border:1px solid rgba(129,104,157,0.25)}
    .panel{background:linear-gradient(180deg, rgba(255,255,255,0.02), rgba(0,0,0,0.06));padding:12px;border-radius:var(--radius);border:1px solid rgba(255,255,255,0.03)}
    .table-wrap{overflow:auto;border-radius:10px;margin-top:12px}
    table{border-collapse:separate;border-spacing:0;width:100%;min-width:900px}
    th,td{padding:8px 10px;text-align:left;border-bottom:1px solid rgba(255,255,255,0.03)}
    thead th{position:sticky;top:0;background:linear-gradient(180deg, rgba(20,20,28,0.9), rgba(18,18,22,0.9));backdrop-filter: blur(4px);z-index:2}
    th.name{width:200px}
    td.name{font-weight:600}
    td[contenteditable]{background:var(--cell);border-radius:6px}
    .muted{color:var(--muted);font-size:13px}
    .small{font-size:13px}
    .row-total{font-weight:700;color:var(--accent-2)}
    tfoot td{font-weight:700;border-top:1px solid rgba(255,255,255,0.04)}
    .actions{display:flex;gap:8px;align-items:center}
    input[type=date], input[type=text]{background:transparent;border:1px dashed rgba(255,255,255,0.03);padding:8px;border-radius:8px;color:inherit}
    .toggle{display:flex;align-items:center;gap:8px}
    .help{font-size:13px;color:var(--muted)}
    .import-area{width:100%;min-height:60px;background:#0b0b0f;border-radius:8px;padding:8px;color:var(--muted);resize:vertical}
    footer{margin-top:12px;color:var(--muted);font-size:13px}
    @media (max-width:820px){.wrap{padding:10px}}
  </style>
</head>
<body>
  <div class="wrap">
    <header>
      <div>
        <h1>Informatika SSD — Collection Tracker</h1>
        <div class="muted small">Dark mode • Dates auto: 8 Oct → 31 Oct 2025 (weekdays only) • Saved in localStorage</div>
      </div>
      <div class="controls">
        <div class="actions panel">
          <button id="addStudent" class="primary">+ Add Student</button>
          <button id="addDateBtn">+ Add Date</button>
          <input id="newDate" type="date" title="Pick a date to add (weekdays only)" />
          <button id="sortName">Sort: Name (A→Z)</button>
          <button id="sortTotal">Sort: Total (High→Low)</button>
          <label class="toggle"><input id="toggleTotals" type="checkbox" /> Show Totals</label>
        </div>
      </div>
    </header>

    <section class="panel">
      <div style="display:flex;gap:12px;flex-direction:column">
        <div style="display:flex;gap:12px;align-items:center">
          <div style="flex:1">
            <div class="help">Import CSV (Name, then values for each date). Paste rows separated by newline. Empty cells allowed.</div>
            <textarea id="importCSV" class="import-area" placeholder="Example:\nAnsel,1.000,1.000, ,1.000"></textarea>
          </div>
          <div style="width:220px;display:flex;flex-direction:column;gap:8px">
            <button id="importBtn">Import CSV</button>
            <button id="exportBtn">Export CSV</button>
            <button id="clearBtn">Clear All Data</button>
          </div>
        </div>
      </div>
    </section>

    <div class="table-wrap panel" id="tablePanel">
      <table id="dataTable">
        <thead id="tableHead"></thead>
        <tbody id="tableBody"></tbody>
        <tfoot id="tableFoot"></tfoot>
      </table>
    </div>

    <footer>
      <div>Usage tips: Click a cell to edit (press Enter or click outside to save). Totals option shows row totals and overall total. You can add weekdays with the date picker. Data kept in your browser only.</div>
      <div style="margin-top:6px" class="muted">localStorage key: <code>ssd-collection-2025-10</code></div>
    </footer>
  </div>

<script>
// Config
const LS_KEY = 'ssd-collection-2025-10';
const startDate = new Date('2025-10-08');
const endDate = new Date('2025-10-31');

// State
let dates = [];
let students = []; // {name: string, values: {dateISO: string: string}} 
let showTotals = false;

// DOM
const tableHead = document.getElementById('tableHead');
const tableBody = document.getElementById('tableBody');
const tableFoot = document.getElementById('tableFoot');
const toggleTotals = document.getElementById('toggleTotals');

// Helpers
function isWeekday(d){ const day = d.getDay(); return day!==0 && day!==6 }
function fmtDateShort(d){ return d.toLocaleDateString(undefined,{day:'2-digit',month:'short'}); }
function iso(d){ return d.toISOString().slice(0,10); }

function generateDates(){
  const arr = [];
  let d = new Date(startDate);
  while(d <= endDate){ if(isWeekday(d)) arr.push(new Date(d)); d.setDate(d.getDate()+1) }
  return arr.map(x=>iso(x));
}

function load(){
  const raw = localStorage.getItem(LS_KEY);
  dates = generateDates();
  if(raw){ try{ const parsed = JSON.parse(raw); if(parsed.dates) dates = parsed.dates; if(parsed.students) students = parsed.students; if(parsed.showTotals!=null) showTotals = parsed.showTotals;}catch(e){console.warn('Failed to parse storage') } }
  toggleTotals.checked = showTotals;
  render();
}

function save(){
  localStorage.setItem(LS_KEY, JSON.stringify({dates,students,showTotals}));
}

function ensureStudentValues(student){
  student.values = student.values || {};
  for(const d of dates) if(!(d in student.values)) student.values[d] = '';
}

function computeRowTotal(student){
  let sum = 0;
  for(const d of dates){ const v = student.values[d]; if(!v) continue; // allow dot thousand sep
    const num = parseNumber(v); if(!isNaN(num)) sum += num
  }
  return sum;
}

function parseNumber(str){
  if(str==null) return NaN;
  const s = String(str).replace(/\./g,'').replace(/,/g,'.').trim();
  return Number(s);
}

function formatNumber(n){
  if(n===''||n==null||isNaN(n)) return '';
  // format with thousand separator '.' and 3 decimals trimmed
  const int = Math.floor(n);
  const frac = (n - int) ? (n - int).toFixed(3).replace(/0+$/,'').replace(/\.$/,'') : '';
  const intS = int.toString().replace(/\B(?=(\d{3})+(?!\d))/g, '.');
  return frac? intS + ',' + frac.slice(2) : intS;
}

// Render
function render(){
  // head
  tableHead.innerHTML = '';
  const tr = document.createElement('tr');
  tr.innerHTML = `<th class="name">Name</th>` + dates.map(d=>`<th data-date="${d}">${fmtDateShort(new Date(d))}</th>`).join('') + (showTotals? '<th>Row Total</th>' : '');
  tableHead.appendChild(tr);

  // body
  tableBody.innerHTML = '';
  for(const s of students){ ensureStudentValues(s); const tr = document.createElement('tr');
    const nameTd = document.createElement('td'); nameTd.className='name'; nameTd.textContent = s.name;
    tr.appendChild(nameTd);
    for(const d of dates){ const td = document.createElement('td'); td.contentEditable = true; td.dataset.date = d; td.dataset.name = s.name; td.textContent = s.values[d] || '';
      td.addEventListener('blur', onCellEdit);
      td.addEventListener('keydown', (e)=>{ if(e.key==='Enter'){ e.preventDefault(); td.blur(); } });
      tr.appendChild(td);
    }
    if(showTotals){ const total = computeRowTotal(s); const td = document.createElement('td'); td.className='row-total'; td.textContent = formatNumber(total); tr.appendChild(td); }
    tableBody.appendChild(tr);
  }

  // footer
  tableFoot.innerHTML = '';
  if(showTotals){ const trf = document.createElement('tr'); trf.innerHTML = `<td class="name">Overall Total</td>` + dates.map(d=>`<td>${formatNumber(totalForDate(d))}</td>`).join('') + `<td class="row-total">${formatNumber(overallTotal())}</td>`; tableFoot.appendChild(trf); }
  save();
}

function onCellEdit(e){ const td = e.target; const name = td.dataset.name; const date = td.dataset.date; const student = students.find(s=>s.name===name); if(!student) return; student.values[date] = td.textContent.trim(); render(); }

function totalForDate(d){ let sum=0; for(const s of students){ const v = s.values[d]; const n = parseNumber(v); if(!isNaN(n)) sum+=n } return sum }
function overallTotal(){ let sum=0; for(const s of students) sum+=computeRowTotal(s); return sum }

// Controls
document.getElementById('addStudent').addEventListener('click', ()=>{
  const name = prompt('Student name:'); if(!name) return; const st = {name: name.trim(), values:{}}; students.push(st); for(const d of dates) st.values[d]=''; render();});

document.getElementById('addDateBtn').addEventListener('click', ()=>{ const v = document.getElementById('newDate').value; if(!v){ alert('Pick a date first'); return; } const d = new Date(v); if(!isWeekday(d)){ alert('That date is weekend — only weekdays allowed'); return; } const isoD = iso(d); if(dates.includes(isoD)){ alert('Date already exists'); return; } // insert sorted
  dates.push(isoD); dates.sort(); for(const s of students) s.values[isoD]=''; render(); });

toggleTotals.addEventListener('change', ()=>{ showTotals = toggleTotals.checked; render(); });

document.getElementById('sortName').addEventListener('click', ()=>{ students.sort((a,b)=> a.name.localeCompare(b.name)); render(); });

document.getElementById('sortTotal').addEventListener('click', ()=>{ students.sort((a,b)=> computeRowTotal(b)-computeRowTotal(a)); render(); });

// Import / Export
document.getElementById('importBtn').addEventListener('click', ()=>{
  const text = document.getElementById('importCSV').value.trim(); if(!text) return alert('Paste CSV rows first');
  // Expected: lines of comma separated values: Name, val1, val2, ... values correspond to current dates
  const lines = text.split('\n').map(l=>l.trim()).filter(Boolean);
  for(const line of lines){ const parts = line.split(',').map(p=>p.trim()); const name = parts.shift(); if(!name) continue; const obj = {name, values:{}}; for(let i=0;i<dates.length;i++){ obj.values[dates[i]] = parts[i]||''; } students.push(obj);
  }
  render(); document.getElementById('importCSV').value=''; alert('Imported '+lines.length+' row(s)');
});

document.getElementById('exportBtn').addEventListener('click', ()=>{
  const header = ['Name', ...dates.map(d=>fmtDateShort(new Date(d)))].join(',');
  const rows = students.map(s=> [s.name, ...dates.map(d=> s.values[d]||'')].join(','));
  const csv = [header, ...rows].join('\n');
  // download
  const blob = new Blob([csv],{type:'text/csv'}); const url = URL.createObjectURL(blob);
  const a = document.createElement('a'); a.href = url; a.download = 'ssd-collection.csv'; document.body.appendChild(a); a.click(); a.remove(); URL.revokeObjectURL(url);
});

document.getElementById('clearBtn').addEventListener('click', ()=>{ if(confirm('Clear all local data? This cannot be undone.')){ students=[]; dates = generateDates(); save(); render(); } });

// Initial load
load();

// Make table cells update student mapping when name changed (not implemented: rename)

</script>
</body>
</html>
