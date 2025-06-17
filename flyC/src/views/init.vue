<script setup>
import { ref, onMounted } from 'vue'
import { request } from '@/common/request'
import { 
    Spin, 
    message, 
    Card, 
    Button, 
    List, 
    Badge, 
    Descriptions, 
    Space, 
    Typography,
    Progress,
    Input,
    Form
} from 'ant-design-vue'
import { invoke } from '@tauri-apps/api/core'

const { Title, Text } = Typography

const emit = defineEmits(['inited'])

// 响应式数据
const loading = ref(true)
const scanStatus = ref('正在获取本机IP地址...')
const networkInfo = ref(null)
const foundDevices = ref([])
const scanProgress = ref({
    current: 0,
    total: 256,
    currentIP: '',
    phase: ''
})
const manualIP = ref('192.168.1.7') // 默认设置为用户提到的IP

// 获取本机IP地址
const getLocalIP = async () => {
    try {
        scanStatus.value = '正在获取本机IP地址...'
        const result = await invoke('get_local_ip')
        networkInfo.value = result
        console.log('本机IP信息:', result)
        return result
    } catch (error) {
        console.error('获取本机IP失败:', error)
        message.error('获取本机IP失败: ' + error)
        return null
    }
}

// 检查设备是否为飞机
const checkDroneDevice = async (ip) => {
    try {
        console.log(`正在检测飞机设备: ${ip}:80`)
        
        // 只尝试访问飞机的扫描接口 /scan (端口80)
        const response = await request({ 
            url: `http://${ip}/scan`,
            timeout: 2000
        })
        
        if (response) {
            console.log(`✅ 发现飞机设备: ${ip}:80`, response)
            return { ip, isActive: true, response, port: 80 }
        }
    } catch (error) {
        console.log(`❌ 设备 ${ip}:80 无响应`)
    }
    return null
}

// 扫描网段中的所有IP（0-255），找到一个就停止
const scanNetworkRange = async (networkBase) => {
    scanStatus.value = `正在扫描网段 ${networkBase}.0-255`
    scanProgress.value.current = 0
    scanProgress.value.total = 256
    scanProgress.value.phase = '扫描IP地址'
    
    // 找到第一个飞机就停止扫描
    for (let i = 0; i < 256; i++) {
        const ip = `${networkBase}.${i}`
        scanProgress.value.currentIP = ip
        scanProgress.value.current = i + 1
        
        // 跳过本机IP
        if (ip === networkInfo.value?.ip_address) {
            continue
        }
        
        console.log(`扫描进度: ${i + 1}/256 - 正在检测 ${ip}`)
        
        const result = await checkDroneDevice(ip)
        if (result) {
            console.log(`🎉 找到飞机设备，停止扫描: ${result.ip}:${result.port}`)
            message.success(`发现飞机设备: ${result.ip}:${result.port}`)
            return [result] // 返回找到的设备，停止扫描
        }
    }
    
    return [] // 扫描完整个网段都没找到
}

// 手动检测指定IP
const checkManualIP = async () => {
    if (!manualIP.value) {
        message.error('请输入IP地址')
        return
    }
    
    loading.value = true
    scanStatus.value = `正在检测手动输入的IP: ${manualIP.value}`
    
    try {
        const result = await checkDroneDevice(manualIP.value)
        if (result) {
            foundDevices.value = [result]
            message.success(`成功连接到飞机: ${result.ip}:${result.port}`)
        } else {
            message.warning(`IP ${manualIP.value} 无法识别为飞机设备`)
        }
    } catch (error) {
        message.error('检测失败: ' + error)
    } finally {
        loading.value = false
    }
}

// 主扫描函数
const scanNet = async () => {
    try {
        loading.value = true
        
        // 1. 获取本机IP
        const localIP = await getLocalIP()
        if (!localIP) {
            scanStatus.value = '无法获取本机IP地址'
            return
        }
        
        scanStatus.value = `本机IP: ${localIP.ip_address}，准备扫描网段 ${localIP.network_base}.0-255`
        
        // 2. 扫描整个网段 (0-255)
        const drones = await scanNetworkRange(localIP.network_base)
        
        if (drones.length > 0) {
            scanStatus.value = `找到飞机设备，停止扫描`
            // 自动连接找到的飞机
            message.success(`自动连接到飞机: ${drones[0].ip}:${drones[0].port}`)
            emit('inited', drones[0])
        } else {
            scanStatus.value = '扫描完成，未发现飞机设备'
            message.warning(`在网段 ${localIP.network_base}.0-255 中未发现飞机设备`)
        }
        
    } catch (error) {
        console.error('扫描失败:', error)
        message.error('扫描失败: ' + error)
        scanStatus.value = '扫描失败'
    } finally {
        loading.value = false
    }
}

// 手动重新扫描
const rescan = () => {
    foundDevices.value = []
    scanProgress.value = { current: 0, total: 256, currentIP: '', phase: '' }
    scanNet()
}

// 连接飞机
const connectDrone = (device) => {
    message.success(`连接到飞机: ${device.ip}:${device.port || 80}`)
    emit('inited', device)
}

// 组件挂载时开始扫描
onMounted(() => {
    scanNet()
})
</script>

<template>
    <div style="padding: 24px; max-width: 800px; margin: 0 auto;">
        <Spin size="large" :spinning="loading" :tip="scanStatus">
            <Space direction="vertical" size="large" style="width: 100%;">
                
                <!-- 手动输入IP -->
                <Card title="手动连接飞机" size="small">
                    <Form layout="inline">
                        <Form.Item label="飞机IP地址">
                            <Input 
                                v-model:value="manualIP" 
                                placeholder="例如: 192.168.1.7"
                                style="width: 200px;"
                            />
                        </Form.Item>
                        <Form.Item>
                            <Button type="primary" @click="checkManualIP" :loading="loading">
                                连接
                            </Button>
                        </Form.Item>
                    </Form>
                </Card>
                
                <!-- 扫描进度显示 -->
                <Card v-if="loading && scanProgress.total > 0" title="扫描进度" size="small">
                    <Space direction="vertical" style="width: 100%;">
                        <Progress 
                            :percent="Math.round((scanProgress.current / scanProgress.total) * 100)"
                            :status="loading ? 'active' : 'success'"
                        />
                        <Descriptions size="small" :column="1">
                            <Descriptions.Item label="扫描阶段">
                                {{ scanProgress.phase }}
                            </Descriptions.Item>
                            <Descriptions.Item label="当前IP">
                                <Text code>{{ scanProgress.currentIP }}</Text>
                            </Descriptions.Item>
                            <Descriptions.Item label="进度">
                                {{ scanProgress.current }} / {{ scanProgress.total }}
                            </Descriptions.Item>
                        </Descriptions>
                    </Space>
                </Card>
                
                <!-- 本机IP信息 -->
                <Card v-if="networkInfo" title="本机IP信息" size="small">
                    <Descriptions size="small" :column="2">
                        <Descriptions.Item label="本机IP">
                            <Text code>{{ networkInfo.ip_address }}</Text>
                        </Descriptions.Item>
                        <Descriptions.Item label="扫描网段">
                            <Text code>{{ networkInfo.network_base }}.0-255</Text>
                        </Descriptions.Item>
                    </Descriptions>
                </Card>
                
                <!-- 发现的飞机设备 -->
                <Card v-if="foundDevices.length > 0" size="small">
                    <template #title>
                        <Space>
                            <Title :level="4" style="margin: 0;">发现的飞机设备</Title>
                            <Badge :count="foundDevices.length" show-zero style="background-color: #52c41a;" />
                        </Space>
                    </template>
                    
                    <List 
                        :data-source="foundDevices" 
                        size="small"
                    >
                        <template #renderItem="{ item }">
                            <List.Item>
                                <template #actions>
                                    <Button type="primary" size="small" @click="connectDrone(item)">
                                        连接
                                    </Button>
                                </template>
                                <List.Item.Meta>
                                    <template #title>
                                        <Space>
                                            <Text strong>{{ item.ip }}:{{ item.port || 80 }}</Text>
                                            <Badge status="success" text="在线" />
                                        </Space>
                                    </template>
                                    <template #description>
                                        飞机设备 - {{ item.path || '/scan' }} 接口可访问
                                    </template>
                                </List.Item.Meta>
                            </List.Item>
                        </template>
                    </List>
                </Card>
                
                <!-- 操作按钮 -->
                <Card v-if="!loading" size="small">
                    <Space style="width: 100%; justify-content: center;">
                        <Button type="default" @click="rescan" :loading="loading">
                            重新扫描整个网段 (0-255)
                        </Button>
                    </Space>
                </Card>
                
                <!-- 空状态提示 -->
                <Card v-if="!loading && foundDevices.length === 0 && networkInfo" size="small">
                    <div style="text-align: center; padding: 40px 0;">
                        <Title :level="4" type="secondary">未发现飞机设备</Title>
                        <Text type="secondary">已扫描网段 {{ networkInfo.network_base }}.0-255</Text>
                        <br>
                        <Text type="secondary">或者使用上方的手动连接功能</Text>
                        <br><br>
                        <Button type="primary" @click="rescan">
                            重新扫描
                        </Button>
                    </div>
                </Card>
            </Space>
        </Spin>
    </div>
</template>

<style scoped>
/* 几乎不需要自定义CSS，全部使用antdv组件的默认样式 */
</style>