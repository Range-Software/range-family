// Range Family icon set generator: one visual language for all UI icons.
// Shares the grid, palette, glyphs and badges with the Range FEA icon set
// (range-fea/src/fea/pixmaps/generate_icons.js) so both applications look alike.
// Grid 24x24, stroke 1.5, round caps/joins, light tinted fills, shared corner badges.
// Generates light icons into this directory and dark color scheme variants into ./dark.
// Usage: node generate_icons.js
const fs = require('fs');
const path = require('path');
const OUT = __dirname;

// Palettes: light is used on light backgrounds, dark on dark color scheme.
// Every color has one meaning: ink = neutral objects, blue = files/navigation,
// violet = AI, green = create/ok, red = remove/error, amber = edit/warning.
// *L are tinted fills, ring/on are badge knock-out/glyph colors.
const THEMES = {
  light: {
    ink: '#64748B', paper: '#F5F7FA', hole: '#FFFFFF', ring: '#FFFFFF', on: '#FFFFFF',
    blue: '#2F7BD0', blueL: '#DCEAFB', blueXL: '#F0F6FD',
    green: '#2E9E5B', greenL: '#DAF2E3',
    red: '#D9534F', redL: '#FBE3E2',
    amber: '#E09A10', amberL: '#FFF0C7',
    violet: '#7B61C9', violetL: '#EAE4F8',
  },
  dark: {
    ink: '#A3AFBD', paper: '#2F353D', hole: '#24292F', ring: '#262A2F', on: '#1B1E22',
    blue: '#5DA4F0', blueL: '#1F3654', blueXL: '#222C39',
    green: '#4FC482', greenL: '#193A28',
    red: '#F07470', redL: '#4A2424',
    amber: '#F2B23C', amberL: '#453413',
    violet: '#A68FF2', violetL: '#2D2549',
  },
};

function build(C) {
const f = n => +n.toFixed(2);
const P = (d, s, fill = 'none', x = '') => `<path d="${d}" stroke="${s}" fill="${fill}"${x}/>`;
const F = (d, fill) => `<path d="${d}" fill="${fill}" stroke="none"/>`;
const circ = (cx, cy, r, s, fill = 'none', x = '') => `<circle cx="${cx}" cy="${cy}" r="${r}" stroke="${s}" fill="${fill}"${x}/>`;
const dot = (cx, cy, r, fill) => `<circle cx="${cx}" cy="${cy}" r="${r}" fill="${fill}" stroke="none"/>`;
const rect = (x, y, w, h, rx, s, fill = 'none', xx = '') => `<rect x="${x}" y="${y}" width="${w}" height="${h}" rx="${rx}" stroke="${s}" fill="${fill}"${xx}/>`;
const thin = ' stroke-width="1"';

// Arrow from (x1,y1) to (x2,y2) with an open head.
function arrow(x1, y1, x2, y2, s, h = 2.6) {
  const a = Math.atan2(y2 - y1, x2 - x1), w = 0.6;
  const l = [x2 - h * Math.cos(a - w), y2 - h * Math.sin(a - w)];
  const r = [x2 - h * Math.cos(a + w), y2 - h * Math.sin(a + w)];
  return P(`M${f(x1)} ${f(y1)}L${f(x2)} ${f(y2)}M${f(l[0])} ${f(l[1])}L${f(x2)} ${f(y2)}L${f(r[0])} ${f(r[1])}`, s);
}

function gearPath(cx, cy, ro, ri, teeth) {
  let d = '';
  const n = teeth * 4;
  for (let i = 0; i < n; i++) {
    const a = (i / n) * 2 * Math.PI - Math.PI / 2 + Math.PI / n / 2;
    const r = (i % 4 === 0 || i % 4 === 1) ? ro : ri;
    d += (i ? 'L' : 'M') + f(cx + r * Math.cos(a)) + ' ' + f(cy + r * Math.sin(a));
  }
  return d + 'Z';
}
const gear = (cx, cy, ro, ri, rh, teeth, s, fill) => P(gearPath(cx, cy, ro, ri, teeth), s, fill) + circ(cx, cy, rh, s, C.hole);

// Four-pointed AI sparkle centered at (cx,cy) with radius r.
const sparkle = (cx, cy, r, s, fill) => {
  const k = r * 0.22;
  return P(`M${f(cx)} ${f(cy - r)}C${f(cx + k)} ${f(cy - k)} ${f(cx + k)} ${f(cy - k)} ${f(cx + r)} ${f(cy)}`
    + `C${f(cx + k)} ${f(cy + k)} ${f(cx + k)} ${f(cy + k)} ${f(cx)} ${f(cy + r)}`
    + `C${f(cx - k)} ${f(cy + k)} ${f(cx - k)} ${f(cy + k)} ${f(cx - r)} ${f(cy)}`
    + `C${f(cx - k)} ${f(cy - k)} ${f(cx - k)} ${f(cy - k)} ${f(cx)} ${f(cy - r)}z`, s, fill);
};

// ---- Badges (bottom-right, knock-out ring) ----
const BX = 18, BY = 18;
const badgeBase = c => `<circle cx="${BX}" cy="${BY}" r="5" fill="${c}" stroke="${C.ring}" stroke-width="1.5"/>`;
const W = C.on;
const badges = {
  plus: () => badgeBase(C.green) + P('M18 15.6v4.8M15.6 18h4.8', W),
  remove: () => badgeBase(C.red) + P('M16.3 16.3l3.4 3.4M19.7 16.3l-3.4 3.4', W),
  edit: () => badgeBase(C.amber) + F('M15.5 20.5l.45-2.05 3.3-3.3 1.6 1.6-3.3 3.3z', W),
  update: () => badgeBase(C.green) + P('M20.3 18a2.3 2.3 0 1 1-.8-1.75M19.9 15.2v1.3h-1.3', W, 'none', ' stroke-width="1.25"'),
  replace: () => badgeBase(C.amber) + P('M15.6 16.8h4.6l-1.2-1.2M20.4 19.2h-4.6l1.2 1.2', W, 'none', ' stroke-width="1.25"'),
  cloud: () => badgeBase(C.blue) + F('M16.2 20.2a1.45 1.45 0 0 1-.1-2.9 2 2 0 0 1 3.75-.6 1.75 1.75 0 0 1 .1 3.5z', W),
  gear: () => badgeBase(C.ink) + F(gearPath(BX, BY, 3.6, 2.6, 6), W) + dot(BX, BY, 1.1, C.ink),
};
const withBadge = (base, b) => base + badges[b]();

// ---- Base glyphs ----
const G = {
  // Document page with folded corner, left edge at x.
  doc: (x = 5.75, s = C.ink, fill = C.paper) => P(`M${x} 2.75h8.5l4 4v14.5h-12.5z`, s, fill) + P(`M${x + 8.5} 2.75v4h4`, s),
  docLines: (x = 5.75) => P(`M${x + 2.75} 11h7M${x + 2.75} 14h7M${x + 2.75} 17h4.5`, C.ink, 'none', thin),
  folder: () => P('M2.75 6.25a1.5 1.5 0 0 1 1.5-1.5h4.5l2 2h9a1.5 1.5 0 0 1 1.5 1.5v9.5a1.5 1.5 0 0 1-1.5 1.5h-15.5a1.5 1.5 0 0 1-1.5-1.5z', C.amber, C.amberL)
    + P('M2.75 9.75h18.5', C.amber),
  floppy: () => P('M3.75 5.25a1.5 1.5 0 0 1 1.5-1.5h11.25l3.75 3.75v11.25a1.5 1.5 0 0 1-1.5 1.5h-13.5a1.5 1.5 0 0 1-1.5-1.5z', C.blue, C.blueL)
    + P('M7.75 3.75v4h7.5v-4', C.blue, C.hole) + P('M7.25 20.25v-6h9.5v6', C.blue, C.hole),
  session: () => P('M3.5 12.25L12 16.5l8.5-4.25', C.ink) + P('M3.5 16.25L12 20.5l8.5-4.25', C.ink)
    + P('M12 3.5l8.5 4.25L12 12 3.5 7.75z', C.blue, C.blueL),
  tray: () => P('M3.75 14.75v3.5a2 2 0 0 0 2 2h12.5a2 2 0 0 0 2-2v-3.5', C.ink),
  power: s => P('M12 3.5v8', s) + P('M7.2 6.3a7.5 7.5 0 1 0 9.6 0', s),
  trash: s => P('M4.5 6.75h15M9.75 6.75v-2a1 1 0 0 1 1-1h2.5a1 1 0 0 1 1 1v2', s)
    + P('M6.25 6.75l.9 12.6a1.5 1.5 0 0 0 1.5 1.4h6.7a1.5 1.5 0 0 0 1.5-1.4l.9-12.6', s, C.paper) + P('M10 10.5v6.5M14 10.5v6.5', s, 'none', thin),
  pencil: () => P('M14.75 4.25l5 5L9 20H4v-5z', C.amber, C.amberL) + P('M12.25 6.75l5 5', C.amber) + P('M4 15l5 5', C.amber, 'none', thin),
  bubble: () => P('M5.25 3.75h13.5a2.5 2.5 0 0 1 2.5 2.5v8a2.5 2.5 0 0 1-2.5 2.5H11.5l-4.75 3.75v-3.75h-1.5a2.5 2.5 0 0 1-2.5-2.5v-8a2.5 2.5 0 0 1 2.5-2.5z', C.violet, C.violetL),
  aiChat: () => G.bubble() + sparkle(12, 10.25, 4.25, C.violet, C.violet),
};

// ---- Icons ----
const I = {};

// Application
I.settings = gear(12, 12, 9, 6.75, 2.75, 8, C.ink, C.paper);
I.quit = G.power(C.red);
I.startup = G.power(C.green);
I.help = circ(12, 12, 8.75, C.blue, C.blueL) + P('M9.6 9.6a2.5 2.5 0 1 1 3.4 2.3c-.6.3-1 .8-1 1.5v.6', C.blue) + dot(12, 16.8, 1.05, C.blue);
I.about = circ(12, 12, 8.75, C.blue, C.blueL) + P('M12 11v5.5', C.blue) + dot(12, 7.9, 1.1, C.blue);
I.update = circ(12, 12, 8.75, C.green, C.greenL) + arrow(12, 7, 12, 16.5, C.green);
I.license = G.doc() + P('M8.5 7h4M8.5 10h7', C.ink, 'none', thin)
  + P('M13.6 17.2l-1 4.3 2.9-1.4 2.9 1.4-1-4.3', C.amber, C.amberL) + circ(15.5, 15, 3, C.amber, C.amberL);
I.release_notes = G.doc() + [10, 13.5, 17].map(y => dot(9, y, 1, C.blue)).join('')
  + P('M11.25 10h4.5M11.25 13.5h4.5M11.25 17h3', C.ink, 'none', thin);

// Action
I.cancel = circ(12, 12, 8.75, C.red, C.redL) + P('M9 9l6 6M15 9l-6 6', C.red);
I.ok = circ(12, 12, 8.75, C.green, C.greenL) + P('M8 12.4l2.7 2.7 5.3-5.6', C.green);
I.close = P('M6 6l12 12M18 6L6 18', C.ink);
I.clear = G.trash(C.ink);
I.undo = P('M8.5 13.5L4 9l4.5-4.5', C.blue) + P('M4 9h10a5.5 5.5 0 0 1 0 11h-3', C.blue);
I.remove = circ(12, 12, 8.75, C.red, C.redL) + P('M8 12h8', C.red);
I.add = circ(12, 12, 8.75, C.green, C.greenL) + P('M12 8v8M8 12h8', C.green);
I.edit = G.pencil();
I.refresh = P('M19.5 12a7.5 7.5 0 0 1-13.1 5', C.blue) + P('M4.5 12a7.5 7.5 0 0 1 13.1-5', C.blue)
  + P('M18.25 3.5v3.75H14.5', C.blue) + P('M5.75 20.5v-3.75H9.5', C.blue);
I.generic_action = P('M13.25 2.75L5 13.5h6.25l-1.5 7.75L19 10.25h-6.25z', C.amber, C.amberL);

// File
I.new = withBadge(G.doc() + G.docLines(), 'plus');
I.open = G.folder();
I.save = G.floppy();
I.save_as = withBadge(G.floppy(), 'edit');
I.import = G.doc(8.75) + arrow(2.5, 14, 13.5, 14, C.blue);
I.export = G.doc(2.75) + arrow(10.5, 14, 21.5, 14, C.blue);
I.compare = P('M2.75 2.75h6l2 2v8.5h-8z', C.ink, C.paper) + P('M13.25 2.75h6l2 2v8.5h-8z', C.ink, C.paper)
  + P('M5 7h3.5M5 9.5h3.5M15.5 7h3.5M15.5 9.5h3.5', C.ink, 'none', thin)
  + arrow(4.5, 17, 19.5, 17, C.blue, 2.2) + arrow(19.5, 20.5, 4.5, 20.5, C.blue, 2.2);

// Cloud
I.access_rights = rect(4.75, 10.25, 14.5, 10.5, 1.5, C.amber, C.amberL) + P('M8 10.25V7.5a4 4 0 0 1 8 0v2.75', C.amber)
  + P('M12 14.5v2.5', C.amber) + dot(12, 14.3, 1.3, C.amber);
I.download = G.tray() + arrow(12, 3.5, 12, 14.5, C.blue);
I.upload = G.tray() + arrow(12, 14.5, 12, 3.5, C.blue);
I.upload_replace = withBadge(G.tray() + arrow(10.5, 14.5, 10.5, 3.5, C.blue), 'replace');
I.upload_update = withBadge(G.tray() + arrow(10.5, 14.5, 10.5, 3.5, C.blue), 'update');
I.file_manager = withBadge(G.folder(), 'cloud');
I.session_manager = withBadge(G.session(), 'cloud');
I.ai_query = withBadge(G.aiChat(), 'cloud');

// AI
I.ai_chat = G.aiChat();
I.ai_settings_manager = withBadge(sparkle(10, 10, 7.25, C.violet, C.violetL) + sparkle(19, 4.75, 2.25, C.violet, C.violet), 'gear');

return I;
}

// ---- Write ----
const wrap = body => `<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24">`
  + `<g fill="none" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">${body}</g></svg>
`;
// Brand artwork is not generated: application logo (and its Windows variant) and the Qt logo.
const skip = ['range-app.svg', 'range-family.svg', 'range-family-windows.svg', 'range-qt.svg'];
for (const [theme, dir] of [['light', OUT], ['dark', path.join(OUT, 'dark')]]) {
  const I = build(THEMES[theme]);
  fs.mkdirSync(dir, { recursive: true });
  for (const [k, body] of Object.entries(I)) fs.writeFileSync(path.join(dir, `range-${k}.svg`), wrap(body));
  console.log(theme, Object.keys(I).length, 'icons written to', dir);
}
for (const file of fs.readdirSync(OUT).filter(x => x.endsWith('.svg') && !skip.includes(x)))
  if (!(file.slice(6, -4) in build(THEMES.light))) console.error('Not generated:', file);
