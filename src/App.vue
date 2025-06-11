<script setup>
import { onMounted } from 'vue'
import nipplejs from 'nipplejs'

onMounted(() => {
    const nipple = nipplejs.create({
        zone: document.querySelector('.control'),
        threshold: 0.5,
    })
    nipple.on('move', (event, data) => {
        console.log('aaaaaaaaa')
    })
    console.log('aaaaaaaaaaaaaa')
})

const see = () => {
    console.log('aaaaaaaaaaaaaa')
}
</script>

<template>
    <main class="container">
        <div ref="nipple" class="control" @click.stop="see"></div>
    </main>
</template>

<style lang="less">
body {
    margin: 0;
    padding: 0;
}
.container {
    position: relative;
    width: 100vw;
    height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    .control {
        position: absolute;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: #9988887d;
    }
}
</style>
