const express = require('express');
const { exec } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
app.use(cors());
app.use(express.json());

const runExe = (fileName, args) => {
    const exePath = path.resolve(__dirname, 'bin', fileName);
    const command = `"${exePath}" ${args.join(' ')}`;
    return new Promise((resolve, reject) => {
        exec(command, (error, stdout, stderr) => {
            if (error) reject(stderr || error.message);
            else resolve(stdout.trim());
        });
    });
};

const xorHex = (hex1, hex2) => {
    const b1 = BigInt('0x' + hex1);
    const b2 = BigInt('0x' + hex2);
    return (b1 ^ b2).toString(16).padStart(Math.max(hex1.length, hex2.length), '0');
};

// 通用的分组加密处理逻辑 (DES & AES)
async function handleBlockCipher(exeName, body) {
    let { data, key, judge, mode, iv, nk, nb } = body;
    const blockSize = exeName === 'DES.exe' ? 16 : (nb * 8);
    
    // 补零逻辑
    while (data.length % blockSize !== 0) data += "0";
    
    const blocks = data.match(new RegExp(`.{1,${blockSize}}`, 'g'));
    const results = [];
    let prev = iv || "0".repeat(blockSize);

    for (let block of blocks) {
        const isEncrypt = judge ? "1" : "0";
        if (mode === 'ECB') {
            const args = exeName === 'DES.exe' ? [block, key, isEncrypt] : [block, key, isEncrypt, nk, nb];
            results.push(await runExe(exeName, args));
        } else {
            if (judge) { // CBC 加密
                const args = exeName === 'DES.exe' ? [xorHex(block, prev), key, "1"] : [xorHex(block, prev), key, "1", nk, nb];
                let cipher = await runExe(exeName, args);
                results.push(cipher);
                prev = cipher;
            } else { // CBC 解密
                const args = exeName === 'DES.exe' ? [block, key, "0"] : [block, key, "0", nk, nb];
                let plain = await runExe(exeName, args);
                results.push(xorHex(plain, prev));
                prev = block;
            }
        }
    }
    return results.join('').toUpperCase();
}

// ==================== 路由部分 ====================

// DES 路由
app.post('/api/des', async (req, res) => {
    try {
        const result = await handleBlockCipher('DES.exe', req.body);
        res.json({ result });
    } catch (err) { res.status(500).json({ error: err }); }
});

// AES 路由
app.post('/api/aes', async (req, res) => {
    try {
        const result = await handleBlockCipher('AES.exe', req.body);
        res.json({ result });
    } catch (err) { res.status(500).json({ error: err }); }
});

// EEA 路由
app.post('/api/eea', async (req, res) => {
    try {
        const result = await runExe('EEA.exe', [req.body.a, req.body.b, req.body.mode]);
        res.json({ result });
    } catch (err) { res.status(500).json({ error: err }); }
});

// RSA 路由
app.post('/api/rsa/gen', async (req, res) => {
    try {
        const { p, q } = req.body;
        const result = await runExe('RSA.exe', ['gen', p, q]);
        const [n, e, d] = result.split(',');
        res.json({ n, e, d });
    } catch (err) { res.status(500).json({ error: err }); }
});

app.post('/api/rsa/encrypt', async (req, res) => {
    try {
        const { data, key, n } = req.body;
        const result = await runExe('RSA.exe', ['calc', data, key, n]);
        res.json({ result });
    } catch (err) { res.status(500).json({ error: err }); }
});

// ==================== ElGamal 新增路由 ====================

// ElGamal 密钥生成
app.post('/api/elgamal/gen', async (req, res) => {
    try {
        const { p, a } = req.body;
        // 调用: ElGamal.exe gen [p] [a]
        const result = await runExe('ElGamal.exe', ['gen', p, a]);
        
        // 解析输出: p,alpha,h
        const [out_p, alpha, h] = result.split(',');
        res.json({ p: out_p, alpha, h });
    } catch (err) { res.status(500).json({ error: String(err) }); }
});

// ElGamal 加密
app.post('/api/elgamal/encrypt', async (req, res) => {
    try {
        const { m, p, alpha, h, k } = req.body;
        // 调用: ElGamal.exe enc [m] [p] [alpha] [h] [k]
        const result = await runExe('ElGamal.exe', ['enc', m, p, alpha, h, k]);
        
        // 解析输出: gamma,delta
        const [gamma, delta] = result.split(',');
        res.json({ gamma, delta });
    } catch (err) { res.status(500).json({ error: String(err) }); }
});

// ElGamal 解密
app.post('/api/elgamal/decrypt', async (req, res) => {
    try {
        const { p, gamma, delta, a } = req.body;
        // 调用: ElGamal.exe dec [p] [gamma] [delta] [a]
        const result = await runExe('ElGamal.exe', ['dec', p, gamma, delta, a]);
        
        // 解析输出: 只有明文 m
        res.json({ m: result });
    } catch (err) { res.status(500).json({ error: String(err) }); }
});
app.post('/api/dh/pub', async (req, res) => {
    try {
        const { p, g, priv } = req.body;
        // 调用: DH.exe pub [p] [g] [priv]
        const result = await runExe('DH.exe', ['pub', p, g, priv]);
        
        if (result.includes('Error')) throw new Error(result);
        res.json({ pub: result });
    } catch (err) { res.status(500).json({ error: String(err) }); }
});

// DH 计算共享会话密钥
app.post('/api/dh/secret', async (req, res) => {
    try {
        const { p, otherPub, myPriv } = req.body;
        // 调用: DH.exe secret [p] [otherPub] [myPriv]
        const result = await runExe('DH.exe', ['secret', p, otherPub, myPriv]);
        
        if (result.includes('Error')) throw new Error(result);
        
        // 解析输出: 原始秘密, 派生16进制密钥
        const [rawSecret, sessionKeyHex] = result.split(',');
        res.json({ rawSecret, sessionKeyHex });
    } catch (err) { res.status(500).json({ error: String(err) }); }
});
app.listen(3000, () => console.log('Server running on port 3000 (Crypto Services Active)'));