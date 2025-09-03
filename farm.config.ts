import { defineConfig } from "@farmfe/core";
import tailwind from "@farmfe/js-plugin-tailwindcss";

export default defineConfig({
  compilation: {
    persistentCache: false,
    output: {
      targetEnv: 'browser-esnext',
      format: 'esm',
    },
  },
  // Additional plugins
  plugins: [tailwind()],
});
