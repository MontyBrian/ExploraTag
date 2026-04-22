import { createRouter, createWebHistory } from 'vue-router'

import Dashboard from '../views/Dashboard.vue'
import Impressum from '../views/Impressum.vue'
import LoginView from "../components/LoginView.vue";
import CallbackView from "../components/CallbackView.vue";

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
    },
    {
        path: "/login",
        name: "login",
        component: LoginView
    },
    {
        path: "/callback",
        name: "callback",
        component: CallbackView
    },
]

const router = createRouter({
    history: createWebHistory(),
    routes
})
export default router;

