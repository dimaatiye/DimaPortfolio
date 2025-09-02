const cacheName = 'C00301176-Dima-Atiyeh-LabSheet5';
const filesToCache = [
  '.',
  'assets/js/scripts.js',
  'index.html',
  'assets/css/style.css',
  './C00301176-Dima-Atiyeh-LabSheet5/',
  './Dima-Atiyeh-c00301176-UI-Programming-Module-Project/assets/js/scripts.js',
  './Dima-Atiyeh-c00301176-UI-Programming-Module-Project/index.html',
  './Dima-Atiyeh-c00301176-UI-Programming-Module-Project/assets/css/style.css',

];

self.addEventListener('install', async e => {
  const cache = await caches.open(cacheName);
  await cache.addAll(filesToCache);
  return self.skipWaiting();
});

self.addEventListener('activate', e => {
  self.clients.claim();
});

self.addEventListener('fetch', async e => {
  const req = e.request;
  const url = new URL(req.url);

  if (url.origin === location.origin) {
    e.respondWith(cacheFirst(req));
  } else {
    e.respondWith(networkAndCache(req));
  }
});

async function cacheFirst(req) {
  const cache = await caches.open(cacheName);
  const cached = await cache.match(req.url);
  return cached || fetch(req);
}

async function networkAndCache(req) {
  const cache = await caches.open(cacheName);
  try {
    const fresh = await fetch(req);
    await cache.put(req, fresh.clone());
    return fresh;
  } catch (e) {
    const cached = await cache.match(req);
    return cached;
  }
}