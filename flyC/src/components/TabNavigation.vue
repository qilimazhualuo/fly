<template>
    <div class="tab-dropdown">
        <select :value="activeTab" @change="$emit('tabChange', $event.target.value)" class="tab-select">
            <option v-for="tab in tabs" :key="tab.id" :value="tab.id">
                {{ tab.icon }} {{ tab.name }}
            </option>
        </select>
    </div>
</template>

<script setup>
defineProps({
    activeTab: {
        type: String,
        required: true
    },
    tabs: {
        type: Array,
        default: () => [
            { id: 'flight', name: '飞行', icon: '✈️' },
            { id: 'config', name: '配置', icon: '⚙️' },
            { id: 'map', name: '地图', icon: '🗺️' }
        ]
    }
})

defineEmits(['tabChange'])
</script>

<style lang="less" scoped>
.tab-dropdown {
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 8px 15px;
    background: rgba(0, 0, 0, 0.3);
    backdrop-filter: blur(10px);
    border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.tab-select {
    background: rgba(68, 68, 68, 0.9);
    color: #fff;
    border: 1px solid rgba(255, 255, 255, 0.2);
    border-radius: 6px;
    padding: 6px 12px;
    font-size: 12px;
    cursor: pointer;
    outline: none;
    backdrop-filter: blur(5px);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
    transition: all 0.3s ease;

    &:hover {
        background: rgba(33, 150, 243, 0.8);
        border-color: rgba(33, 150, 243, 0.8);
    }

    &:focus {
        border-color: #2196F3;
        box-shadow: 0 0 8px rgba(33, 150, 243, 0.5);
    }

    option {
        background: rgba(26, 26, 26, 0.95);
        color: #fff;
        padding: 6px;
    }
}
</style>