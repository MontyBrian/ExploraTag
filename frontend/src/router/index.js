import { createRouter, createWebHistory } from 'vue-router'

import Dashboard from '../views/Dashboard.vue'
import Impressum from '../views/Impressum.vue'

const routes = [
    {
        path: '/',
        name: 'Dashboard',
        component: Dashboard
    },
    {
        path: '/impressum',
        name: 'Impressum',
        component: Impressum
    }
]

const router = createRouter({
    history: createWebHistory(),
    routes
})

export default router