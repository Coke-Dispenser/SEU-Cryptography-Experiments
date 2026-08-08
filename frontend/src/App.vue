<template>
  <div class="crypto-container">
    <el-card>
      <template #header>
        <div class="header">密码学实验</div>
      </template>
      <el-tabs v-model="activeTab" type="border-card">
        
        <el-tab-pane label="扩展欧几里得" name="EEA">
          <el-form :model="eeaForm" label-position="top">
            <el-row :gutter="20">
              <el-col :span="12">
                <el-form-item label="整数 a">
                  <el-input v-model.number="eeaForm.a" type="number" />
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="整数 b (或模数 m)">
                  <el-input v-model.number="eeaForm.b" type="number" />
                </el-form-item>
              </el-col>
            </el-row>
            <el-form-item>
              <el-button type="primary" @click="handleEEA(0)">求解 GCD</el-button>
              <el-button type="success" @click="handleEEA(1)">求解模逆元</el-button>
            </el-form-item>
            <el-form-item label="计算结果">
              <el-input v-model="eeaForm.result" readonly>
                <template #prepend>{{ eeaForm.lastMode === 0 ? 'GCD' : 'Inverse' }}</template>
              </el-input>
            </el-form-item>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="DES 算法" name="DES">
          <el-form :model="desForm" label-position="top">
            <el-form-item label="密钥 (16位16进制)">
              <el-input v-model="desForm.key" maxlength="16" show-word-limit />
            </el-form-item>
            <el-form-item label="工作模式">
              <el-radio-group v-model="desForm.mode">
                <el-radio label="ECB">ECB</el-radio>
                <el-radio label="CBC">CBC</el-radio>
              </el-radio-group>
            </el-form-item>
            <el-form-item label="初始向量 IV (16位16进制)" v-if="desForm.mode === 'CBC'">
              <el-input v-model="desForm.iv" maxlength="16" />
            </el-form-item>
            <el-form-item label="数据内容 (16进制)">
              <el-input v-model="desForm.data" type="textarea" :rows="3" />
            </el-form-item>
            <el-form-item>
              <el-button type="primary" @click="handleDES(true)">加密</el-button>
              <el-button type="success" @click="handleDES(false)">解密</el-button>
            </el-form-item>
            <el-form-item label="结果">
              <el-input v-model="desForm.result" readonly type="textarea">
                <template #append>
                  <el-button @click="copy(desForm.result)">复制</el-button>
                </template>
              </el-input>
            </el-form-item>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="AES 算法" name="AES">
          <el-form :model="aesForm" label-position="top">
            <el-row :gutter="20">
              <el-col :span="12">
                <el-form-item label="密钥长度 (Nk)">
                  <el-select v-model="aesForm.nk">
                    <el-option label="AES-128 (Nk=4)" :value="4" />
                    <el-option label="AES-192 (Nk=6)" :value="6" />
                    <el-option label="AES-256 (Nk=8)" :value="8" />
                  </el-select>
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="分组长度 (Nb)">
                  <el-select v-model="aesForm.nb">
                    <el-option label="标准 (Nb=4)" :value="4" />
                    <el-option label="扩展 (Nb=6)" :value="6" />
                    <el-option label="扩展 (Nb=8)" :value="8" />
                  </el-select>
                </el-form-item>
              </el-col>
            </el-row>
            <el-form-item :label="`密钥 (${aesForm.nk * 8}位16进制)`">
              <el-input v-model="aesForm.key" :maxlength="aesForm.nk * 8" show-word-limit />
            </el-form-item>
            <el-form-item label="工作模式">
              <el-radio-group v-model="aesForm.mode">
                <el-radio label="ECB">ECB</el-radio>
                <el-radio label="CBC">CBC</el-radio>
              </el-radio-group>
            </el-form-item>
            <el-form-item :label="`初始向量 IV (${aesForm.nb * 8}位16进制)`" v-if="aesForm.mode === 'CBC'">
              <el-input v-model="aesForm.iv" :maxlength="aesForm.nb * 8" />
            </el-form-item>
            <el-form-item label="数据内容 (16进制)">
              <el-input v-model="aesForm.data" type="textarea" :rows="3" />
            </el-form-item>
            <el-form-item>
              <el-button type="primary" @click="handleAES(true)">加密</el-button>
              <el-button type="success" @click="handleAES(false)">解密</el-button>
            </el-form-item>
            <el-form-item label="结果">
              <el-input v-model="aesForm.result" readonly type="textarea">
                <template #append>
                  <el-button @click="copy(aesForm.result)">复制</el-button>
                </template>
              </el-input>
            </el-form-item>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="RSA 算法" name="RSA">
          <el-form :model="rsaForm" label-position="top">
            
            
            <el-row :gutter="20">
              <el-col :span="8">
                <el-form-item label="质数 p">
                  <el-input v-model.number="rsaForm.p" type="number" placeholder="输入大质数" />
                </el-form-item>
              </el-col>
              <el-col :span="8">
                <el-form-item label="质数 q">
                  <el-input v-model.number="rsaForm.q" type="number" placeholder="输入大质数" />
                </el-form-item>
              </el-col>
              <el-col :span="8">
                <el-form-item label="密钥操作">
                  <el-button type="warning" @click="handleRSAGen" style="width: 100%">生成密钥对</el-button>
                </el-form-item>
              </el-col>
            </el-row>

            <el-row :gutter="20">
              <el-col :span="8">
                <el-form-item label="模数 n (p*q)">
                  <el-input v-model="rsaForm.n" />
                </el-form-item>
              </el-col>
              <el-col :span="8">
                <el-form-item label="公钥指数 e">
                  <el-input v-model="rsaForm.e" />
                </el-form-item>
              </el-col>
              <el-col :span="8">
                <el-form-item label="私钥指数 d">
                  <el-input v-model="rsaForm.d" />
                </el-form-item>
              </el-col>
            </el-row>

            <el-divider content-position="left">加解密计算 (混合模式)</el-divider>

            <el-form-item label="明文/密文数据 (16进制)">
              <el-input v-model="rsaForm.data" type="textarea" :rows="3" placeholder="加密：输入任意长度16进制明文&#10;解密：输入包含 EncKey: 和 |Cipher: 格式的混合密文" />
            </el-form-item>

            <el-form-item>
              <el-button type="primary" @click="handleRSACalc(true)">公钥加密</el-button>
              <el-button type="success" @click="handleRSACalc(false)">私钥解密 </el-button>
            </el-form-item>

            <el-form-item label="计算结果">
              <el-input v-model="rsaForm.result" readonly type="textarea" :rows="4">
                <template #append>
                  <el-button @click="copy(rsaForm.result)">复制</el-button>
                </template>
              </el-input>
            </el-form-item>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="ElGamal 算法" name="ElGamal">
          <el-form :model="elgamalForm" label-position="top">
            
            

            <el-divider content-position="left">密钥生成</el-divider>
            <el-row :gutter="20">
              <el-col :span="12">
                <el-form-item label="素数 p (必须是素数)">
                  <el-input v-model.number="elgamalForm.p" type="number" placeholder="例如: 23" />
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="私钥 a">
                  <el-input v-model.number="elgamalForm.a" type="number" placeholder="例如: 6" />
                </el-form-item>
              </el-col>
            </el-row>
            <el-form-item>
              <el-button type="warning" @click="handleElGamalGen" style="width: 100%">生成生成元(alpha)与公钥(h)</el-button>
            </el-form-item>

            <el-row :gutter="20">
              <el-col :span="12">
                <el-form-item label="生成元 alpha">
                  <el-input v-model="elgamalForm.alpha" readonly placeholder="点击上方生成" />
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="公钥 h">
                  <el-input v-model="elgamalForm.h" readonly placeholder="点击上方生成" />
                </el-form-item>
              </el-col>
            </el-row>

            <el-divider content-position="left">加解密计算 (混合模式)</el-divider>
            
            <el-form-item label="明文/密文数据 (16进制)">
              <el-input v-model="elgamalForm.data" type="textarea" :rows="3" placeholder="加密：输入任意长度16进制明文&#10;解密：输入包含 EncKey: 和 |Cipher: 格式的混合密文" />
            </el-form-item>

            <el-form-item label="随机数 k (1 <= k <= p-2) [仅加密使用]">
              <el-input v-model.number="elgamalForm.k" type="number" placeholder="例如: 3" />
            </el-form-item>

            <el-form-item>
              <el-button type="primary" @click="handleElGamalEnc">加密</el-button>
              <el-button type="success" @click="handleElGamalDec">解密</el-button>
            </el-form-item>

            <el-form-item label="解密/加密结果">
              <el-input v-model="elgamalForm.result" readonly type="textarea" :rows="4">
                <template #append>
                  <el-button @click="copy(elgamalForm.result)">复制</el-button>
                </template>
              </el-input>
            </el-form-item>

          </el-form>
        </el-tab-pane>
        <el-tab-pane label="Diffie-Hellman 协商" name="DH">
          <el-form :model="dhForm" label-position="top">
            <el-divider content-position="left">1. 公共参数 (公开信道)</el-divider>
            <el-row :gutter="20">
              <el-col :span="12">
                <el-form-item label="大素数 p">
                  <el-input v-model.number="dhForm.p" type="number" placeholder="例如: 97" />
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="原根/生成元 g">
                  <el-input v-model.number="dhForm.g" type="number" placeholder="例如: 5" />
                </el-form-item>
              </el-col>
            </el-row>

            <el-divider content-position="left">2. 各自的私钥与公钥计算</el-divider>
            <el-row :gutter="20">
              <el-col :span="12">
                <el-card shadow="hover">
                  <template #header><b>Alice 的端点</b></template>
                  <el-form-item label="Alice 的私钥 a (保密)">
                    <el-input v-model.number="dhForm.a" type="number" />
                  </el-form-item>
                  <el-form-item label="Alice 的公钥 A (发给 Bob)">
                    <el-input v-model="dhForm.A" readonly placeholder="点击下方计算生成" />
                  </el-form-item>
                </el-card>
              </el-col>
              <el-col :span="12">
                <el-card shadow="hover">
                  <template #header><b>Bob 的端点</b></template>
                  <el-form-item label="Bob 的私钥 b (保密)">
                    <el-input v-model.number="dhForm.b" type="number" />
                  </el-form-item>
                  <el-form-item label="Bob 的公钥 B (发给 Alice)">
                    <el-input v-model="dhForm.B" readonly placeholder="点击下方计算生成" />
                  </el-form-item>
                </el-card>
              </el-col>
            </el-row>

            <el-form-item style="margin-top: 20px;">
              <el-button type="primary" @click="handleDHPublicKeys" style="width: 100%">分别计算 Alice 和 Bob 的公钥 (A 和 B)</el-button>
            </el-form-item>

            <el-divider content-position="left">3. 协商会话密钥</el-divider>
            <el-form-item>
              <el-button type="warning" @click="handleDHSecret" style="width: 100%">双方计算最终会话密钥</el-button>
            </el-form-item>

            <el-row :gutter="20">
              <el-col :span="12">
                <el-form-item label="Alice 计算出的原始秘密 (S)">
                  <el-input v-model="dhForm.secretAlice" readonly />
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="Bob 计算出的原始秘密 (S)">
                  <el-input v-model="dhForm.secretBob" readonly />
                </el-form-item>
              </el-col>
            </el-row>

            <!-- 
            <el-form-item label="派生出的最终会话密钥 (256-bit Hex，可直接用于AES)">
              <el-input v-model="dhForm.finalKeyHex" readonly type="textarea" :rows="2">
                <template #append>
                  <el-button @click="copy(dhForm.finalKeyHex)">复制</el-button>
                </template>
              </el-input>
            </el-form-item>
            -->

          </el-form>
        </el-tab-pane>
      </el-tabs>
    </el-card>
  </div>
</template>

<script setup>
import { reactive, ref, watch } from 'vue'
import { ElMessage } from 'element-plus'
import axios from 'axios'

const activeTab = ref('ElGamal')

const aesForm = reactive({
  key: '',
  data: '',
  result: '',
  mode: 'ECB',
  nk: 4,
  nb: 4,
  iv: '00000000000000000000000000000000'
})

const desForm = reactive({
  key: '',
  data: '',
  result: '',
  mode: 'ECB',
  iv: '0000000000000000'
})

const eeaForm = reactive({
  a: 0,
  b: 0,
  result: '',
  lastMode: 0
})

watch(() => aesForm.nb, (val) => {
  aesForm.iv = '0'.repeat(val * 8)
})

const handleAES = async (isEncrypt) => {
  try {
    const res = await axios.post('http://localhost:3000/api/aes', { ...aesForm, judge: isEncrypt })
    aesForm.result = res.data.result
    ElMessage.success('AES 操作成功')
  } catch (e) { ElMessage.error('AES 操作失败') }
}

const handleDES = async (isEncrypt) => {
  try {
    const res = await axios.post('http://localhost:3000/api/des', { ...desForm, judge: isEncrypt })
    desForm.result = res.data.result
    ElMessage.success('DES 操作成功')
  } catch (e) { ElMessage.error('DES 操作失败') }
}

const handleEEA = async (mode) => {
  eeaForm.lastMode = mode
  try {
    const res = await axios.post('http://localhost:3000/api/eea', { a: eeaForm.a, b: eeaForm.b, mode })
    eeaForm.result = res.data.result === "-1" ? "无逆元" : res.data.result
  } catch (e) { ElMessage.error('EEA 操作失败') }
}

// ==================== 改造后的 RSA ====================
const rsaForm = reactive({
  p: 61,
  q: 53,
  n: '',
  e: '',
  d: '',
  data: '',
  result: ''
})

const handleRSAGen = async () => {
  if (!rsaForm.p || !rsaForm.q) {
    ElMessage.warning('请输入质数 p 和 q')
    return
  }
  try {
    const res = await axios.post('http://localhost:3000/api/rsa/gen', { p: rsaForm.p, q: rsaForm.q })
    rsaForm.n = res.data.n
    rsaForm.e = res.data.e
    rsaForm.d = res.data.d
    ElMessage.success('密钥生成成功')
  } catch (e) {
    ElMessage.error('密钥生成失败，请确保 p, q 为质数')
  }
}

const handleRSACalc = async (isEncrypt) => {
  if (!rsaForm.data || !rsaForm.n) {
    ElMessage.warning('请确保数据和模数 n 不为空')
    return
  }
  
  try {
    if (isEncrypt) {
      // 1. 生成一次性 128-bit AES 密钥（32个16进制字符）
      const tempAesKey = Array.from({ length: 32 }, () => Math.floor(Math.random() * 16).toString(16)).join('');

      // 2. 利用 RSA 公钥对 AES 密钥按字符进行安全分块加密（规避N过小导致的溢出）
      const encryptPromises = Array.from({ length: 32 }, (_, i) => {
        const charVal = parseInt(tempAesKey[i], 16);
        return axios.post('http://localhost:3000/api/rsa/encrypt', {
          data: charVal.toString(),
          key: rsaForm.e,
          n: rsaForm.n
        });
      });
      const encResults = await Promise.all(encryptPromises);
      const encryptedKeyStr = encResults.map(r => r.data.result).join(',');

      // 3. 利用 AES-128 (ECB) 加密原本的任意长度大数据
      const aesRes = await axios.post('http://localhost:3000/api/aes', {
        data: rsaForm.data,
        key: tempAesKey,
        judge: true,
        mode: 'ECB',
        nk: 4,
        nb: 4
      });

      // 4. 将数字信封组合呈现
      rsaForm.result = `EncKey:${encryptedKeyStr}|Cipher:${aesRes.data.result}`;
      ElMessage.success('混合加密完成 (RSA保护密钥，AES加密数据)');
      
    } else {
      // 解密流程
      const input = rsaForm.data;
      const match = input.match(/^EncKey:(.*?)\|Cipher:(.*)$/);
      if (!match) {
        ElMessage.error('解密失败：数据格式不正确，必须包含 EncKey 和 Cipher。');
        return;
      }
      
      const encryptedKeyStr = match[1];
      const cipherText = match[2];

      // 1. 利用 RSA 私钥批量解密恢复 AES 密钥
      const encKeyParts = encryptedKeyStr.split(',');
      const decryptPromises = encKeyParts.map(part => {
        return axios.post('http://localhost:3000/api/rsa/encrypt', {
          data: part,
          key: rsaForm.d,
          n: rsaForm.n
        });
      });
      const decResults = await Promise.all(decryptPromises);
      const tempAesKey = decResults.map(r => parseInt(r.data.result, 10).toString(16)).join('');

      // 2. 利用恢复出的 AES 密钥还原明文大数据
      const aesRes = await axios.post('http://localhost:3000/api/aes', {
        data: cipherText,
        key: tempAesKey,
        judge: false,
        mode: 'ECB',
        nk: 4,
        nb: 4
      });

      rsaForm.result = aesRes.data.result;
      ElMessage.success('混合解密完成');
    }
  } catch (e) {
    ElMessage.error('RSA 混合计算失败，请检查数据完整性');
  }
}

// ==================== 改造后的 ElGamal ====================
const elgamalForm = reactive({
  p: 23,   
  a: 6,    
  alpha: '',
  h: '',
  data: '', 
  k: 3,
  result: ''
})

const handleElGamalGen = async () => {
  if (!elgamalForm.p || !elgamalForm.a) {
    ElMessage.warning('请输入素数 p 和私钥 a')
    return
  }
  try {
    const res = await axios.post('http://localhost:3000/api/elgamal/gen', {
      p: elgamalForm.p,
      a: elgamalForm.a
    })
    elgamalForm.alpha = res.data.alpha
    elgamalForm.h = res.data.h
    ElMessage.success('ElGamal 生成元及公钥生成成功')
  } catch (e) {
    const errMsg = e.response?.data?.error || '生成失败，请检查 p 是否为素数'
    ElMessage.error(errMsg)
  }
}

const handleElGamalEnc = async () => {
  if (!elgamalForm.data || !elgamalForm.p || !elgamalForm.alpha || !elgamalForm.h || !elgamalForm.k) {
    ElMessage.warning('请确保明文数据、随机数 k 填写完整，且已生成密钥')
    return
  }
  try {
    // 1. 生成一次性 128-bit AES 密钥（32个16进制字符）
    const tempAesKey = Array.from({ length: 32 }, () => Math.floor(Math.random() * 16).toString(16)).join('');

    // 2. 使用 ElGamal 保护临时密钥 (按字符拆分保证不超限)，并动态应用有效的随机数k (1<=k<=p-2)
    const baseK = parseInt(elgamalForm.k, 10);
    const p = parseInt(elgamalForm.p, 10);
    
    const encryptPromises = Array.from({ length: 32 }, (_, i) => {
      const charVal = parseInt(tempAesKey[i], 16);
      const currentK = ((baseK + i) % (p - 2)) + 1; // 保证范围内的合法 K
      
      return axios.post('http://localhost:3000/api/elgamal/encrypt', {
        m: charVal,
        p: elgamalForm.p,
        alpha: elgamalForm.alpha,
        h: elgamalForm.h,
        k: currentK
      });
    });
    const encResults = await Promise.all(encryptPromises);
    const encryptedKeyStr = encResults.map(r => `${r.data.gamma}-${r.data.delta}`).join(',');

    // 3. 用生成的 AES 密钥加密大数据
    const aesRes = await axios.post('http://localhost:3000/api/aes', {
      data: elgamalForm.data,
      key: tempAesKey,
      judge: true,
      mode: 'ECB',
      nk: 4,
      nb: 4
    });

    elgamalForm.result = `EncKey:${encryptedKeyStr}|Cipher:${aesRes.data.result}`;
    ElMessage.success('ElGamal 混合加密完成');
  } catch (e) {
    const errMsg = e.response?.data?.error || '加密失败'
    ElMessage.error(errMsg)
  }
}

const handleElGamalDec = async () => {
  if (!elgamalForm.data || !elgamalForm.p || !elgamalForm.a) {
    ElMessage.warning('请确保待解密数据和私钥参数填写完整')
    return
  }
  try {
    const input = elgamalForm.data;
    const match = input.match(/^EncKey:(.*?)\|Cipher:(.*)$/);
    if (!match) {
      ElMessage.error('解密失败：数据格式不符合混合加密特征。');
      return;
    }
    const encryptedKeyStr = match[1];
    const cipherText = match[2];

    // 1. 通过 ElGamal 批量解密 AES 密钥
    const encKeyParts = encryptedKeyStr.split(',');
    const decryptPromises = encKeyParts.map(part => {
      const [gamma, delta] = part.split('-');
      return axios.post('http://localhost:3000/api/elgamal/decrypt', {
        p: elgamalForm.p,
        gamma: gamma,
        delta: delta,
        a: elgamalForm.a
      });
    });
    const decResults = await Promise.all(decryptPromises);
    const tempAesKey = decResults.map(r => parseInt(r.data.m, 10).toString(16)).join('');

    // 2. 用恢复的 AES 密钥解密大数据
    const aesRes = await axios.post('http://localhost:3000/api/aes', {
      data: cipherText,
      key: tempAesKey,
      judge: false,
      mode: 'ECB',
      nk: 4,
      nb: 4
    });

    elgamalForm.result = aesRes.data.result;
    ElMessage.success('ElGamal 混合解密完成');
  } catch (e) {
    const errMsg = e.response?.data?.error || '解密失败'
    ElMessage.error(errMsg)
  }
}

const copy = (text) => {
  if (!text) return
  navigator.clipboard.writeText(text)
  ElMessage.success('已复制到剪贴板')
}

const dhForm = reactive({
  p: 97,    // 默认示例素数
  g: 5,     // 默认示例生成元
  a: 36,    // Alice 私钥
  b: 58,    // Bob 私钥
  A: '',    // Alice 公钥
  B: '',    // Bob 公钥
  secretAlice: '',
  secretBob: '',
  finalKeyHex: ''
})

const handleDHPublicKeys = async () => {
  if (!dhForm.p || !dhForm.g || !dhForm.a || !dhForm.b) {
    ElMessage.warning('请将 p, g, a, b 填写完整')
    return
  }
  try {
    // 并发请求：同时计算 Alice 和 Bob 的公钥
    const [resA, resB] = await Promise.all([
      axios.post('http://localhost:3000/api/dh/pub', { p: dhForm.p, g: dhForm.g, priv: dhForm.a }),
      axios.post('http://localhost:3000/api/dh/pub', { p: dhForm.p, g: dhForm.g, priv: dhForm.b })
    ]);
    
    dhForm.A = resA.data.pub;
    dhForm.B = resB.data.pub;
    
    dhForm.secretAlice = '';
    dhForm.secretBob = '';
    dhForm.finalKeyHex = '';
    ElMessage.success('公钥计算成功！现在可以交换公钥并计算会话密钥了。')
  } catch (e) {
    ElMessage.error(e.response?.data?.error || '公钥计算失败')
  }
}

const handleDHSecret = async () => {
  if (!dhForm.A || !dhForm.B) {
    ElMessage.warning('请先计算双方公钥')
    return
  }
  try {
    // Alice 拿着 Bob 的公钥 B 和自己的私钥 a 计算
    const reqAlice = axios.post('http://localhost:3000/api/dh/secret', { 
      p: dhForm.p, otherPub: dhForm.B, myPriv: dhForm.a 
    });
    // Bob 拿着 Alice 的公钥 A 和自己的私钥 b 计算
    const reqBob = axios.post('http://localhost:3000/api/dh/secret', { 
      p: dhForm.p, otherPub: dhForm.A, myPriv: dhForm.b 
    });

    const [resAlice, resBob] = await Promise.all([reqAlice, reqBob]);

    dhForm.secretAlice = resAlice.data.rawSecret;
    dhForm.secretBob = resBob.data.rawSecret;

    // 验证双方协商结果是否一致
    if (resAlice.data.rawSecret === resBob.data.rawSecret) {
      dhForm.finalKeyHex = resAlice.data.sessionKeyHex;
      ElMessage.success('密钥协商成功！双方得出了完全一致的会话密钥。')
    } else {
      ElMessage.error('严重错误：双方计算出的密钥不一致！')
    }
  } catch (e) {
    ElMessage.error(e.response?.data?.error || '会话密钥计算失败')
  }
}
</script>

<style scoped>
.crypto-container {
  max-width: 900px;
  margin: 20px auto;
}
.header {
  font-weight: bold;
  font-size: 1.5rem;
  text-align: center;
}
.el-form-item {
  margin-bottom: 18px;
}
</style>