#pragma once
// Dashboard HTML for the ProDuckX-DNS web UI, kept in its own header so the
// Arduino IDE preprocessor doesn't choke on the inlined markup (issue #6).
// UI dùng Tailwind (Play CDN) + FontAwesome + Chart.js. Logic JS giữ nguyên.

const char PAGE[] PROGMEM = R"HTML(<!doctype html><html lang=en><head><meta charset=utf-8>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>ProDuckX-DNS</title>
<script src="https://cdn.tailwindcss.com"></script>
<link rel=stylesheet href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.2/css/all.min.css">
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.1/dist/chart.umd.min.js"></script>
</head>
<body class="min-h-screen bg-slate-950 text-slate-200 antialiased">
<header class="sticky top-0 z-20 border-b border-slate-800 bg-slate-900/95 backdrop-blur">
  <div class="mx-auto flex max-w-6xl items-center gap-3 px-4 py-3">
    <span class="flex h-9 w-9 items-center justify-center rounded-xl border border-emerald-500/30 bg-emerald-500/10">
      <i class="fa-solid fa-shield-halved text-emerald-400"></i></span>
    <h1 class="text-base font-semibold tracking-tight">ProDuckX-DNS
      <span id=host class="ml-1 font-mono text-xs font-normal text-slate-400"></span></h1>
    <span class="ml-auto hidden rounded-full border border-slate-700 px-2.5 py-0.5 text-[11px] text-slate-400 sm:inline">
      <i class="fa-solid fa-circle-nodes mr-1 text-emerald-400"></i>DNS sinkhole</span>
  </div>
</header>

<main class="mx-auto max-w-6xl space-y-5 px-4 py-5">
<div id=credwarn class="hidden items-start gap-3 rounded-xl border border-rose-500/40 bg-rose-500/10 px-4 py-3 text-sm text-rose-200">
  <i class="fa-solid fa-triangle-exclamation mt-0.5 text-rose-400"></i>
  <div><b>Default credentials in use.</b> WEB_PASS/OTA_PASS in <code class="rounded bg-slate-900/60 px-1">secrets.h</code>
  are still the placeholder values — anyone can read them in the public repo. Set real values and reflash.</div>
</div>

<div id=sys class="grid grid-cols-2 gap-3"></div>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-chart-pie text-emerald-400"></i>Blocked vs allowed</div>
  <div class="h-56 p-4"><canvas id=ch></canvas></div>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-chart-column text-emerald-400"></i>Requests per client — today</div>
  <div class="h-64 p-4"><canvas id=ch2></canvas></div>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-users text-emerald-400"></i>Clients</div>
  <div class=overflow-x-auto>
    <table id=ct class="w-full text-sm"><thead class="bg-slate-800/60 text-[11px] uppercase tracking-wide text-slate-400">
      <tr><th class="px-4 py-2 text-left">Client</th><th class="px-4 py-2 text-left">MAC</th>
      <th class="px-4 py-2 text-left">Blocked</th><th class="px-4 py-2 text-left">Allowed</th><th class="px-4 py-2"></th></tr>
    </thead><tbody></tbody></table>
  </div>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-ban text-rose-400"></i>Custom blocked domains</div>
  <div class="flex flex-wrap gap-2 px-4 pt-4">
    <input id=dom placeholder="ads.example.com" size=30
      class="w-full max-w-xs rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none">
    <button onclick=addDom() class="rounded-lg bg-emerald-500 px-3 py-2 text-sm font-semibold text-slate-950 hover:bg-emerald-400">
      <i class="fa-solid fa-plus mr-1"></i>Block domain</button>
  </div>
  <div class=overflow-x-auto>
    <table id=cl class="mt-3 w-full text-sm"><tbody></tbody></table>
  </div>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-circle-check text-emerald-400"></i>Whitelist — always allowed</div>
  <div class="flex flex-wrap gap-2 px-4 pt-4">
    <input id=wdom placeholder="analytics.example.com" size=30
      class="w-full max-w-xs rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none">
    <button onclick=addWhite() class="rounded-lg border border-emerald-500/40 bg-emerald-500/10 px-3 py-2 text-sm font-semibold text-emerald-300 hover:bg-emerald-500/20">
      <i class="fa-solid fa-check mr-1"></i>Allow domain</button>
  </div>
  <p class="px-4 pt-2 text-xs text-slate-500">whitelist wins over the blocklist: the domain and its subdomains are never blocked</p>
  <div class=overflow-x-auto>
    <table id=wl class="mt-2 w-full text-sm"><tbody></tbody></table>
  </div>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-file-arrow-up text-emerald-400"></i>Blocklist — upload</div>
  <form id=upf class="flex flex-wrap items-center gap-2 px-4 py-4">
    <input type=file id=blf accept=.bin
      class="rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm text-slate-300 file:mr-3 file:rounded-md file:border-0 file:bg-slate-800 file:px-2 file:py-1 file:text-xs file:text-slate-200">
    <button class="rounded-lg bg-emerald-500 px-3 py-2 text-sm font-semibold text-slate-950 hover:bg-emerald-400">
      <i class="fa-solid fa-upload mr-1"></i>Upload blocklist</button>
    <span id=upmsg class="text-xs text-slate-400"></span>
  </form>
  <p class="px-4 pb-4 text-xs text-slate-500">build <code class="rounded bg-slate-800 px-1">blocklist.bin</code> with
    <code class="rounded bg-slate-800 px-1">tools/build_blocklist.py</code>, then upload here — no USB</p>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-cloud-arrow-down text-emerald-400"></i>Blocklist — remote auto-update</div>
  <div class="flex flex-wrap items-center gap-2 px-4 pt-4">
    <input id=uurl placeholder="https://host/blocklist.bin" size=40
      class="w-full max-w-md rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none">
    <span class="text-xs text-slate-400">every</span>
    <input id=uiv size=2 value=24
      class="w-16 rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm focus:border-emerald-500 focus:outline-none">
    <span class="text-xs text-slate-400">h</span>
    <button onclick=saveUpd() class="rounded-lg border border-slate-700 bg-slate-800 px-3 py-2 text-sm hover:bg-slate-700">Save</button>
    <button onclick=fetchNow() class="rounded-lg border border-slate-700 bg-slate-800 px-3 py-2 text-sm hover:bg-slate-700">Fetch now</button>
  </div>
  <p class="px-4 py-4 text-xs text-slate-500">device pulls a prebuilt <code class="rounded bg-slate-800 px-1">blocklist.bin</code> on a schedule
    (e.g. a GitHub release asset). last: <span id=ustat>—</span></p>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-network-wired text-emerald-400"></i>Network — static IP</div>
  <div class="grid grid-cols-1 gap-3 px-4 pt-4 sm:grid-cols-2 lg:grid-cols-4">
    <div><label class="mb-1 block text-xs text-slate-400">IP Address</label>
      <input id=netip placeholder="192.168.1.50" class="w-full rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none"></div>
    <div><label class="mb-1 block text-xs text-slate-400">Gateway</label>
      <input id=netgw placeholder="192.168.1.1" class="w-full rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none"></div>
    <div><label class="mb-1 block text-xs text-slate-400">Subnet Mask</label>
      <input id=netsn placeholder="255.255.255.0" class="w-full rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none"></div>
    <div><label class="mb-1 block text-xs text-slate-400">DNS Server</label>
      <input id=netdns placeholder="1.1.1.1" class="w-full rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none"></div>
  </div>
  <div class="flex flex-wrap items-center gap-2 px-4 pt-3">
    <button onclick=saveIP() class="rounded-lg bg-emerald-500 px-3 py-2 text-sm font-semibold text-slate-950 hover:bg-emerald-400">Save</button>
    <button onclick=setDhcp() class="rounded-lg border border-slate-700 bg-slate-800 px-3 py-2 text-sm hover:bg-slate-700">Use DHCP</button>
    <span id=netmsg class="text-xs text-slate-400"></span>
  </div>
  <p class="px-4 py-4 text-xs text-slate-500">applies after a reboot. currently: <span id=netstate>—</span></p>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-arrow-up-right-from-square text-emerald-400"></i>Blocked page redirect</div>
  <div class="flex flex-wrap items-center gap-2 px-4 pt-4">
    <input id=burl placeholder="0.0.0.0 (default) or https://block.mydomain.com" size=40
      class="w-full max-w-md rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-sm placeholder-slate-500 focus:border-emerald-500 focus:outline-none">
    <button onclick=saveBlockUrl() class="rounded-lg bg-emerald-500 px-3 py-2 text-sm font-semibold text-slate-950 hover:bg-emerald-400">Save</button>
    <button onclick=resetBlockUrl() class="rounded-lg border border-slate-700 bg-slate-800 px-3 py-2 text-sm hover:bg-slate-700">Reset (0.0.0.0)</button>
    <span id=bmsg class="text-xs text-slate-400"></span>
  </div>
  <p class="px-4 py-4 text-xs text-slate-500">default is 0.0.0.0. Enter a redirect URL (e.g.
    <code class="rounded bg-slate-800 px-1">https://block.mydomain.com</code>) to show when a domain is blocked.</p>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-wifi text-emerald-400"></i>WiFi</div>
  <div class="px-4 py-4">
    <button onclick="if(confirm('Forget saved WiFi and reboot into the setup portal?'))forgetWifi()"
      class="rounded-lg border border-rose-500/40 bg-rose-500/10 px-3 py-2 text-sm font-semibold text-rose-300 hover:bg-rose-500/20">
      <i class="fa-solid fa-plug-circle-xmark mr-1"></i>Forget WiFi</button>
  </div>
</section>

<section class="rounded-xl border border-slate-800 bg-slate-900/60">
  <div class="flex items-center gap-2 border-b border-slate-800 px-4 py-3 text-sm font-medium text-slate-300">
    <i class="fa-solid fa-clock-rotate-left text-emerald-400"></i>Recently blocked</div>
  <div class="max-h-[352px] overflow-y-auto">
    <table id=ht class="w-full text-sm"><thead class="sticky top-0 bg-slate-800 text-[11px] uppercase tracking-wide text-slate-400">
      <tr><th class="px-4 py-2 text-left">Domain</th><th class="px-4 py-2 text-left">Client</th><th class="px-4 py-2 text-left">When</th></tr>
    </thead><tbody></tbody></table>
  </div>
</section>
</main>

<script>
function fmt(n){return n.toLocaleString()}
function esc(s){return String(s).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]))}
// A plain <img>/<form> CSRF can't set a custom header, only same-origin fetch()
// can — so requiring this on every mutating request blocks drive-by CSRF even
// though the server can't otherwise tell a forged request from a real one over
// plain HTTP Basic Auth (browsers auto-replay cached Basic Auth cross-origin).
const CSRF_HDRS={'X-Requested-With':'produckx-adblock'}
// Biểu đồ tròn blocked/allowed, cập nhật từ /stats.json (không đổi logic backend)
let chart=null, chart2=null;
function initChart(){if(chart||typeof Chart==='undefined')return;let c=document.getElementById('ch');if(!c)return;
chart=new Chart(c,{type:'doughnut',data:{labels:['Blocked','Allowed'],datasets:[{data:[0,0],
backgroundColor:['#f43f5e','#10b981'],borderColor:'#0f172a',borderWidth:3}]},
options:{responsive:true,maintainAspectRatio:false,cutout:'68%',
plugins:{legend:{position:'bottom',labels:{color:'#94a3b8',boxWidth:10,usePointStyle:true}}}}});
let c2=document.getElementById('ch2');if(!c2)return;
chart2=new Chart(c2,{type:'bar',data:{labels:[],datasets:[
{label:'Blocked',data:[],backgroundColor:'#f43f5e',stack:'s',borderRadius:4},
{label:'Allowed',data:[],backgroundColor:'#10b981',stack:'s',borderRadius:4}]},
options:{indexAxis:'y',responsive:true,maintainAspectRatio:false,
scales:{x:{stacked:true,ticks:{color:'#94a3b8',precision:0},grid:{color:'rgba(148,163,184,.15)'}},
y:{stacked:true,ticks:{color:'#94a3b8',font:{size:10}},grid:{display:false}}},
plugins:{legend:{position:'bottom',labels:{color:'#94a3b8',boxWidth:10,usePointStyle:true}}}}})}
async function load(){let r=await fetch('/stats.json');if(r.status===401){location.href='/login';return;}let s=await r.json();
host.textContent='@ '+s.ip;
credwarn.style.display=s.defcreds?'flex':'none';
sys.className='grid grid-cols-2 gap-3';
sys.innerHTML=[['Total blocked',fmt(s.blocked),'text-rose-400','fa-ban'],['Total allowed',fmt(s.allowed),'text-emerald-400','fa-circle-check'],['Blocklist domains',fmt(s.domains),'text-slate-100','fa-list'],
['Clients',s.clients.length,'text-slate-100','fa-users'],['CPU',(s.cpu??0)+'%','text-slate-100','fa-microchip'],['RAM',(s.ram??0)+'%','text-slate-100','fa-memory'],['Uptime',s.uptime,'text-slate-100','fa-clock']]
.map((c,i)=>`<div class="rounded-xl border border-slate-800 bg-slate-900/60 px-4 py-3 ${i===6?'col-span-2':''}"><div class="flex items-start justify-between gap-2"><div class="text-2xl font-semibold ${c[2]}">${c[1]}</div><i class="fa-solid ${c[3]} mt-1 text-sm ${c[2]} opacity-70"></i></div><div class="mt-1 text-xs text-slate-400">${c[0]}</div></div>`).join('');
ct.tBodies[0].innerHTML=s.clients.sort((a,b)=>(b.blocked+b.allowed)-(a.blocked+a.allowed)).map(c=>
`<tr class="border-b border-slate-800/70 hover:bg-slate-800/40"><td class="px-4 py-2.5 break-all">${c.ip}${c.banned?' <span class="ml-1 rounded bg-rose-500/15 px-1.5 py-0.5 text-[10px] font-semibold text-rose-400">BANNED</span>':''}</td>
<td class="whitespace-nowrap px-4 py-2.5 font-mono text-xs text-slate-400">${c.mac}</td>
<td class="px-4 py-2.5 text-rose-400">${fmt(c.blocked)}</td><td class="px-4 py-2.5 text-emerald-400">${fmt(c.allowed)}</td>
<td class="px-4 py-2.5 text-right"><button class="ban rounded-md border border-slate-700 bg-slate-800 px-2.5 py-1 text-xs hover:bg-slate-700" data-ip="${c.ip}">${c.banned?'Unban':'Ban'}</button></td></tr>`).join('');
cl.tBodies[0].innerHTML=s.custom.map(d=>`<tr class="border-b border-slate-800/70"><td class="px-4 py-2 break-all">${esc(d)}</td>
<td class="px-4 py-2 text-right"><button class="rmbtn rounded-md border border-slate-700 bg-slate-800 px-2.5 py-1 text-xs text-rose-300 hover:bg-slate-700" data-d="${esc(d)}">remove</button></td></tr>`).join('')||'<tr><td class="px-4 py-3 text-xs text-slate-500">none yet</td></tr>';
wl.tBodies[0].innerHTML=s.white.map(d=>`<tr class="border-b border-slate-800/70"><td class="px-4 py-2 break-all">${esc(d)}</td>
<td class="px-4 py-2 text-right"><button class="wrm rounded-md border border-slate-700 bg-slate-800 px-2.5 py-1 text-xs text-rose-300 hover:bg-slate-700" data-d="${esc(d)}">remove</button></td></tr>`).join('')||'<tr><td class="px-4 py-3 text-xs text-slate-500">none yet</td></tr>';
ht.tBodies[0].innerHTML=s.hist.map(h=>`<tr class="border-b border-slate-800/70 hover:bg-slate-800/40"><td class="px-4 py-2.5 break-all">${esc(h.d)}</td>
<td class="whitespace-nowrap px-4 py-2.5 font-mono text-xs text-slate-400">${h.ip}</td><td class="whitespace-nowrap px-4 py-2.5 text-xs text-slate-400">${h.t<60?h.t+'s ago':Math.round(h.t/60)+'m ago'}</td></tr>`).join('')||'<tr><td colspan=3 class="px-4 py-3 text-xs text-slate-500">nothing blocked yet</td></tr>';
netstate.textContent=s.staticip?'static':'DHCP';
if(!netEdited){netip.value=s.netip||'';netgw.value=s.netgw||'';netsn.value=s.netsubnet||'';netdns.value=s.netdns||'';}
if(document.activeElement!=burl)burl.value=s.blockurl||'';
if(document.activeElement!=uurl)uurl.value=s.upurl||'';
if(document.activeElement!=uiv)uiv.value=s.upiv||24;
ustat.textContent=s.upstat||'—';
if(chart){chart.data.datasets[0].data=[s.blocked,s.allowed];chart.update('none');}
if(chart2){let cs=[...s.clients].sort((a,b)=>(b.today||0)-(a.today||0)).slice(0,8);
chart2.data.labels=cs.map(c=>c.ip);
chart2.data.datasets[0].data=cs.map(c=>c.tblocked||0);
chart2.data.datasets[1].data=cs.map(c=>c.tallowed||0);
chart2.update('none');}}
function addDom(){let d=dom.value.trim();if(d){fetch('/addblock?d='+encodeURIComponent(d),{headers:CSRF_HDRS}).then(()=>{dom.value='';load()})}}
ct.addEventListener('click',e=>{if(e.target.classList.contains('ban'))fetch('/ban?ip='+e.target.dataset.ip,{headers:CSRF_HDRS}).then(load)});
cl.addEventListener('click',e=>{if(e.target.classList.contains('rmbtn'))fetch('/unblock?d='+encodeURIComponent(e.target.dataset.d),{headers:CSRF_HDRS}).then(load)});
function addWhite(){let d=wdom.value.trim();if(d){fetch('/addwhite?d='+encodeURIComponent(d),{headers:CSRF_HDRS}).then(()=>{wdom.value='';load()})}}
wl.addEventListener('click',e=>{if(e.target.classList.contains('wrm'))fetch('/unwhite?d='+encodeURIComponent(e.target.dataset.d),{headers:CSRF_HDRS}).then(load)});
let netEdited=false;
[netip,netgw,netsn,netdns].forEach(el=>el.addEventListener('input',()=>{netEdited=true;}));
function saveIP(){let ip=netip.value.trim(),gw=netgw.value.trim(),sn=netsn.value.trim(),dns=netdns.value.trim();
if(!ip||!gw||!sn||!dns){netmsg.textContent='fill all four fields';return;}
netmsg.textContent='saving...';
netEdited=false;
fetch('/setip?ip='+encodeURIComponent(ip)+'&gw='+encodeURIComponent(gw)+'&subnet='+encodeURIComponent(sn)+'&dns='+encodeURIComponent(dns),{headers:CSRF_HDRS})
.then(async r=>{netmsg.textContent=r.ok?'✓ saved, rebooting to apply':'✗ '+await r.text()}).catch(()=>netmsg.textContent='✓ rebooting...');}
function setDhcp(){if(!confirm('Switch back to DHCP and reboot?'))return;netmsg.textContent='saving...';
fetch('/setdhcp',{headers:CSRF_HDRS}).then(async r=>{netmsg.textContent=r.ok?'✓ rebooting':'✗ '+await r.text()}).catch(()=>netmsg.textContent='✓ rebooting...');}
function saveBlockUrl(){let u=burl.value.trim();bmsg.textContent='saving...';
fetch('/setblockurl?u='+encodeURIComponent(u),{headers:CSRF_HDRS}).then(r=>{bmsg.textContent=r.ok?'✓ saved':'✗ failed';load();});}
function resetBlockUrl(){burl.value='0.0.0.0';saveBlockUrl();}
function saveUpd(){fetch('/setupdate?u='+encodeURIComponent(uurl.value.trim())+'&h='+(parseInt(uiv.value)||24),{headers:CSRF_HDRS}).then(load)}
function fetchNow(){ustat.textContent='fetching...';fetch('/fetchnow',{headers:CSRF_HDRS}).then(r=>r.text()).then(t=>{ustat.textContent=t;load()})}
function forgetWifi(){fetch('/forgetwifi',{headers:CSRF_HDRS}).then(r=>r.text()).then(t=>alert(t))}
upf.onsubmit=async e=>{e.preventDefault();let f=blf.files[0];if(!f)return;
upmsg.textContent='uploading '+(f.size/1048576).toFixed(2)+' MB...';
let fd=new FormData();fd.append('f',f);
try{let r=await fetch('/upload',{method:'POST',headers:CSRF_HDRS,body:fd});upmsg.textContent=r.ok?'✓ updated':'✗ '+await r.text();}
catch(_){upmsg.textContent='✗ upload failed';}
blf.value='';setTimeout(load,600);};
initChart();load();setInterval(load,3000);
</script></body></html>)HTML";
