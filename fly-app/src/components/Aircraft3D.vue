<script setup>
import { ref, onMounted, onUnmounted, watch } from 'vue'
import {
    Scene,
    PerspectiveCamera,
    WebGLRenderer,
    Group,
    CylinderGeometry,
    ConeGeometry,
    SphereGeometry,
    BoxGeometry,
    MeshPhongMaterial,
    Mesh,
    AmbientLight,
    DirectionalLight,
    Color,
    PCFSoftShadowMap
} from 'three'
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js'

const props = defineProps({
    flightData: {
        type: Object,
        required: true,
    },
    controls: {
        type: Object,
        required: true,
    }
})

const canvasRef = ref(null)
let scene, camera, renderer, aircraft, orbitControls
let animationId = null

// 飞机部件引用
let leftAileron, rightAileron, elevator, rudder, propeller

onMounted(() => {
    initThreeJS()
    createAircraft()
    animate()
})

onUnmounted(() => {
    if (animationId) {
        cancelAnimationFrame(animationId)
    }
    if (orbitControls) {
        orbitControls.dispose()
    }
    if (renderer) {
        renderer.dispose()
    }
})

// 监听控制变化
watch(() => props.controls, (newControls) => {
    updateControlSurfaces(newControls)
}, { deep: true })

const initThreeJS = () => {
    // 创建场景
    scene = new Scene()
    scene.background = new Color(0x000814)

    // 创建相机 - 调整为Z轴向上的视角
    camera = new PerspectiveCamera(75, 400 / 250, 0.1, 1000)
    camera.position.set(5, -2, 3) // 从侧前方看飞机
    camera.up.set(0, 0, 1) // 设置Z轴为上方向
    camera.lookAt(0, 0, 0)

    // 创建渲染器
    renderer = new WebGLRenderer({
        canvas: canvasRef.value,
        antialias: true,
        alpha: true
    })
    renderer.setSize(400, 250)
    renderer.shadowMap.enabled = true
    renderer.shadowMap.type = PCFSoftShadowMap

    // 添加轨道控制器
    orbitControls = new OrbitControls(camera, renderer.domElement)
    orbitControls.enableDamping = true
    orbitControls.dampingFactor = 0.05
    orbitControls.enableZoom = true
    orbitControls.enablePan = false
    orbitControls.minDistance = 2
    orbitControls.maxDistance = 10
    // 确保target存在后再设置
    if (orbitControls.target) {
        orbitControls.target.set(0, 0, 0)
    }

    // 添加环境光
    const ambientLight = new AmbientLight(0x404040, 0.6)
    scene.add(ambientLight)

    // 添加方向光 - 调整光照方向
    const directionalLight = new DirectionalLight(0xffffff, 0.8)
    directionalLight.position.set(5, -5, 8) // 从上方照射
    directionalLight.castShadow = true
    scene.add(directionalLight)
}

const createAircraft = () => {
    aircraft = new Group()

    // 材质
    const fuselageMaterial = new MeshPhongMaterial({ color: 0xcccccc })
    const wingMaterial = new MeshPhongMaterial({ color: 0xe0e0e0 })
    const cockpitMaterial = new MeshPhongMaterial({ color: 0x333333 })
    const propMaterial = new MeshPhongMaterial({ color: 0x8b4513 })
    const controlMaterial = new MeshPhongMaterial({ color: 0x999999 })

    // 机身 - X轴向前
    const fuselageGeometry = new CylinderGeometry(0.15, 0.08, 3, 8)
    const fuselage = new Mesh(fuselageGeometry, fuselageMaterial)
    fuselage.rotation.z = Math.PI / 2 // 旋转使其沿X轴
    fuselage.position.set(0, 0, 0)
    aircraft.add(fuselage)

    // 机头 - 指向X轴正方向
    const noseGeometry = new ConeGeometry(0.15, 0.5, 8)
    const nose = new Mesh(noseGeometry, fuselageMaterial)
    nose.rotation.z = -Math.PI / 2
    nose.position.set(1.75, 0, 0)
    aircraft.add(nose)

    // 驾驶舱
    const cockpitGeometry = new SphereGeometry(0.18, 8, 6, 0, Math.PI)
    const cockpit = new Mesh(cockpitGeometry, cockpitMaterial)
    cockpit.position.set(0.8, 0, 0.05)
    cockpit.rotation.z = Math.PI / 2
    aircraft.add(cockpit)

    // 主翼 - 沿Y轴展开
    const wingGeometry = new BoxGeometry(0.4, 2.5, 0.1)
    const leftWing = new Mesh(wingGeometry, wingMaterial)
    leftWing.position.set(0, -1.3, 0)
    aircraft.add(leftWing)

    const rightWing = new Mesh(wingGeometry, wingMaterial)
    rightWing.position.set(0, 1.3, 0)
    aircraft.add(rightWing)

    // 副翼
    const aileronGeometry = new BoxGeometry(0.15, 0.6, 0.05)
    leftAileron = new Mesh(aileronGeometry, controlMaterial)
    leftAileron.position.set(-0.15, -1.8, 0)
    aircraft.add(leftAileron)

    rightAileron = new Mesh(aileronGeometry, controlMaterial)
    rightAileron.position.set(-0.15, 1.8, 0)
    aircraft.add(rightAileron)

    // 水平尾翼
    const hTailGeometry = new BoxGeometry(0.25, 1.2, 0.08)
    const hTail = new Mesh(hTailGeometry, wingMaterial)
    hTail.position.set(-1.2, 0, 0)
    aircraft.add(hTail)

    // 升降舵
    const elevatorGeometry = new BoxGeometry(0.1, 1.0, 0.04)
    elevator = new Mesh(elevatorGeometry, controlMaterial)
    elevator.position.set(-1.35, 0, 0)
    aircraft.add(elevator)

    // 垂直尾翼 - 沿Z轴向上
    const vTailGeometry = new BoxGeometry(0.25, 0.08, 0.8)
    const vTail = new Mesh(vTailGeometry, wingMaterial)
    vTail.position.set(-1.2, 0, 0.3)
    aircraft.add(vTail)

    // 方向舵
    const rudderGeometry = new BoxGeometry(0.1, 0.04, 0.4)
    rudder = new Mesh(rudderGeometry, controlMaterial)
    rudder.position.set(-1.35, 0, 0.5)
    aircraft.add(rudder)

    // 螺旋桨 - 在YZ平面旋转
    const propGeometry = new BoxGeometry(0.02, 1.5, 0.1)
    propeller = new Mesh(propGeometry, propMaterial)
    propeller.position.set(2.1, 0, 0)
    aircraft.add(propeller)

    // 起落架
    const gearMaterial = new MeshPhongMaterial({ color: 0x444444 })
    const gearGeometry = new CylinderGeometry(0.02, 0.02, 0.3)

    const leftGear = new Mesh(gearGeometry, gearMaterial)
    leftGear.position.set(0.3, -0.8, -0.3)
    leftGear.rotation.x = Math.PI / 2
    aircraft.add(leftGear)

    const rightGear = new Mesh(gearGeometry, gearMaterial)
    rightGear.position.set(0.3, 0.8, -0.3)
    rightGear.rotation.x = Math.PI / 2
    aircraft.add(rightGear)

    const tailGear = new Mesh(gearGeometry, gearMaterial)
    tailGear.position.set(-1, 0, -0.3)
    tailGear.rotation.x = Math.PI / 2
    aircraft.add(tailGear)

    // 轮子
    const wheelGeometry = new CylinderGeometry(0.08, 0.08, 0.05)
    const wheelMaterial = new MeshPhongMaterial({ color: 0x222222 })

    const leftWheel = new Mesh(wheelGeometry, wheelMaterial)
    leftWheel.position.set(0.3, -0.8, -0.45)
    leftWheel.rotation.x = Math.PI / 2
    aircraft.add(leftWheel)

    const rightWheel = new Mesh(wheelGeometry, wheelMaterial)
    rightWheel.position.set(0.3, 0.8, -0.45)
    rightWheel.rotation.x = Math.PI / 2
    aircraft.add(rightWheel)

    const tailWheel = new Mesh(wheelGeometry, wheelMaterial)
    tailWheel.position.set(-1, 0, -0.45)
    tailWheel.rotation.x = Math.PI / 2
    aircraft.add(tailWheel)

    scene.add(aircraft)
}

const updateControlSurfaces = (controls) => {
    if (!leftAileron || !rightAileron || !elevator || !rudder || !propeller) return

    // 副翼控制 (左右相反) - 绕X轴旋转
    const aileronAngle = (controls.aileron / 100) * Math.PI / 6 // 最大30度
    leftAileron.rotation.x = -aileronAngle
    rightAileron.rotation.x = aileronAngle

    // 升降舵控制 - 绕Y轴旋转
    const elevatorAngle = (controls.elevator / 100) * Math.PI / 6
    elevator.rotation.y = elevatorAngle

    // 方向舵控制 - 绕Z轴旋转
    const rudderAngle = (controls.rudder / 100) * Math.PI / 6
    rudder.rotation.z = rudderAngle

    // 螺旋桨固定旋转 - 绕X轴旋转
    propeller.rotation.x += 0.3

    // 控制面发光效果
    if (Math.abs(controls.aileron) > 10) {
        leftAileron.material.emissive.setHex(0x0066ff)
        rightAileron.material.emissive.setHex(0x0066ff)
    } else {
        leftAileron.material.emissive.setHex(0x000000)
        rightAileron.material.emissive.setHex(0x000000)
    }

    if (Math.abs(controls.elevator) > 10) {
        elevator.material.emissive.setHex(0x9900cc)
    } else {
        elevator.material.emissive.setHex(0x000000)
    }

    if (Math.abs(controls.rudder) > 10) {
        rudder.material.emissive.setHex(0x996633)
    } else {
        rudder.material.emissive.setHex(0x000000)
    }
}

const animate = () => {
    animationId = requestAnimationFrame(animate)

    // 更新轨道控制器
    if (orbitControls) {
        orbitControls.update()
    }

    if (aircraft) {
        // 飞机轻微摇摆 - 符合航空坐标系
        aircraft.rotation.x = Math.sin(Date.now() * 0.001) * 0.01 // 俯仰 (pitch)
        aircraft.rotation.y = Math.sin(Date.now() * 0.0008) * 0.01 // 偏航 (yaw)
        aircraft.rotation.z = Math.sin(Date.now() * 0.0015) * 0.02 // 滚转 (roll)
    }

    renderer.render(scene, camera)
}
</script>

<template>
    <div class="aircraft-3d">
        <div class="canvas-container">
            <canvas ref="canvasRef"></canvas>
            <div class="interaction-hint">
                <span>🖱️ 拖拽旋转 | 📱 双指缩放</span>
            </div>
        </div>
    </div>
</template>

<style lang="less" scoped>
.aircraft-3d {
    position: relative;
    background: rgba(42, 42, 42, 0.9);
    border-radius: 8px;
    padding: 10px;
    margin-bottom: 10px;
    display: flex;
    flex-direction: column;
    align-items: center;
}

.canvas-container {
    position: relative;
    width: 400px;
    height: 250px;
    border-radius: 6px;
    background: linear-gradient(135deg, #000814 0%, #001122 100%);
    overflow: hidden;
}

canvas {
    width: 100%;
    height: 100%;
    border-radius: 6px;
    cursor: grab;

    &:active {
        cursor: grabbing;
    }
}

.interaction-hint {
    position: absolute;
    top: 8px;
    right: 8px;
    color: rgba(255, 255, 255, 0.6);
    font-size: 10px;
    text-align: center;
    background: rgba(0, 0, 0, 0.5);
    padding: 2px 6px;
    border-radius: 3px;
    pointer-events: none;
    opacity: 0;
    transition: opacity 0.3s ease;
}

.canvas-container:hover .interaction-hint {
    opacity: 1;
}
</style>
