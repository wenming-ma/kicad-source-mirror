# KiCad PCB最小可编译依赖集合分析报告

**基于BOARD和FOOTPRINT核心数据的编译期依赖分析结果**

## 📊 统计摘要

- **最小源文件集合**: 1457 个 .cpp/.cc 文件
- **最小头文件集合**: 4549 个 .h/.hpp 文件
- **未解析符号**: 13538 个 (需要外部库)

## 🔧 最小源文件集合 (.cpp/.cc)

以下源文件构成了BOARD/FOOTPRINT功能的最小编译单元：

  1. `3d-viewer\3d_cache\3d_cache.cpp`
  2. `3d-viewer\3d_cache\3d_plugin_manager.cpp`
  3. `3d-viewer\3d_cache\sg\ifsg_node.cpp`
  4. `3d-viewer\3d_cache\sg\ifsg_transform.cpp`
  5. `3d-viewer\3d_cache\sg\scenegraph.cpp`
  6. `3d-viewer\3d_cache\sg\sg_appearance.cpp`
  7. `3d-viewer\3d_cache\sg\sg_base.cpp`
  8. `3d-viewer\3d_cache\sg\sg_colors.cpp`
  9. `3d-viewer\3d_cache\sg\sg_coordindex.cpp`
 10. `3d-viewer\3d_cache\sg\sg_coords.cpp`
 11. `3d-viewer\3d_cache\sg\sg_faceset.cpp`
 12. `3d-viewer\3d_cache\sg\sg_helpers.cpp`
 13. `3d-viewer\3d_cache\sg\sg_index.cpp`
 14. `3d-viewer\3d_cache\sg\sg_node.cpp`
 15. `3d-viewer\3d_cache\sg\sg_normals.cpp`
 16. `3d-viewer\3d_cache\sg\sg_shape.cpp`
 17. `3d-viewer\3d_canvas\board_adapter.cpp`
 18. `3d-viewer\3d_canvas\create_3Dgraphic_brd_items.cpp`
 19. `3d-viewer\3d_canvas\create_layer_items.cpp`
 20. `3d-viewer\3d_canvas\eda_3d_canvas.cpp`
 21. `3d-viewer\3d_canvas\eda_3d_canvas_pivot.cpp`
 22. `3d-viewer\3d_fastmath.cpp`
 23. `3d-viewer\3d_model_viewer\eda_3d_model_viewer.cpp`
 24. `3d-viewer\3d_navlib\nl_3d_viewer_plugin.cpp`
 25. `3d-viewer\3d_navlib\nl_3d_viewer_plugin_impl.cpp`
 26. `3d-viewer\3d_navlib\nl_footprint_properties_plugin.cpp`
 27. `3d-viewer\3d_navlib\nl_footprint_properties_plugin_impl.cpp`
 28. `3d-viewer\3d_rendering\buffers_debug.cpp`
 29. `3d-viewer\3d_rendering\color_rgba.cpp`
 30. `3d-viewer\3d_rendering\image.cpp`
 31. `3d-viewer\3d_rendering\opengl\3d_model.cpp`
 32. `3d-viewer\3d_rendering\opengl\create_scene.cpp`
 33. `3d-viewer\3d_rendering\opengl\layer_triangles.cpp`
 34. `3d-viewer\3d_rendering\opengl\opengl_utils.cpp`
 35. `3d-viewer\3d_rendering\opengl\render_3d_opengl.cpp`
 36. `3d-viewer\3d_rendering\post_shader.cpp`
 37. `3d-viewer\3d_rendering\post_shader_ssao.cpp`
 38. `3d-viewer\3d_rendering\raytracing\PerlinNoise.cpp`
 39. `3d-viewer\3d_rendering\raytracing\accelerators\accelerator_3d.cpp`
 40. `3d-viewer\3d_rendering\raytracing\accelerators\bvh_packet_traversal.cpp`
 41. `3d-viewer\3d_rendering\raytracing\accelerators\bvh_pbrt.cpp`
 42. `3d-viewer\3d_rendering\raytracing\accelerators\container_2d.cpp`
 43. `3d-viewer\3d_rendering\raytracing\accelerators\container_3d.cpp`
 44. `3d-viewer\3d_rendering\raytracing\create_scene.cpp`
 45. `3d-viewer\3d_rendering\raytracing\frustum.cpp`
 46. `3d-viewer\3d_rendering\raytracing\material.cpp`
 47. `3d-viewer\3d_rendering\raytracing\mortoncodes.cpp`
 48. `3d-viewer\3d_rendering\raytracing\ray.cpp`
 49. `3d-viewer\3d_rendering\raytracing\raypacket.cpp`
 50. `3d-viewer\3d_rendering\raytracing\render_3d_raytrace_base.cpp`
 51. `3d-viewer\3d_rendering\raytracing\render_3d_raytrace_gl.cpp`
 52. `3d-viewer\3d_rendering\raytracing\render_3d_raytrace_ram.cpp`
 53. `3d-viewer\3d_rendering\raytracing\shapes2D\bbox_2d.cpp`
 54. `3d-viewer\3d_rendering\raytracing\shapes2D\filled_circle_2d.cpp`
 55. `3d-viewer\3d_rendering\raytracing\shapes2D\layer_item_2d.cpp`
 56. `3d-viewer\3d_rendering\raytracing\shapes2D\object_2d.cpp`
 57. `3d-viewer\3d_rendering\raytracing\shapes2D\polygon_2d.cpp`
 58. `3d-viewer\3d_rendering\raytracing\shapes2D\ring_2d.cpp`
 59. `3d-viewer\3d_rendering\raytracing\shapes2D\round_segment_2d.cpp`
 60. `3d-viewer\3d_rendering\raytracing\shapes2D\triangle_2d.cpp`
 61. `3d-viewer\3d_rendering\raytracing\shapes3D\bbox_3d.cpp`
 62. `3d-viewer\3d_rendering\raytracing\shapes3D\bbox_3d_ray.cpp`
 63. `3d-viewer\3d_rendering\raytracing\shapes3D\cylinder_3d.cpp`
 64. `3d-viewer\3d_rendering\raytracing\shapes3D\layer_item_3d.cpp`
 65. `3d-viewer\3d_rendering\raytracing\shapes3D\object_3d.cpp`
 66. `3d-viewer\3d_rendering\raytracing\shapes3D\plane_3d.cpp`
 67. `3d-viewer\3d_rendering\raytracing\shapes3D\round_segment_3d.cpp`
 68. `3d-viewer\3d_rendering\raytracing\shapes3D\triangle_3d.cpp`
 69. `3d-viewer\3d_rendering\render_3d_base.cpp`
 70. `3d-viewer\3d_rendering\track_ball.cpp`
 71. `3d-viewer\3d_rendering\trackball.cpp`
 72. `3d-viewer\3d_viewer\3d_menubar.cpp`
 73. `3d-viewer\3d_viewer\3d_toolbar.cpp`
 74. `3d-viewer\3d_viewer\eda_3d_viewer_frame.cpp`
 75. `3d-viewer\3d_viewer\eda_3d_viewer_settings.cpp`
 76. `3d-viewer\3d_viewer\tools\eda_3d_actions.cpp`
 77. `3d-viewer\3d_viewer\tools\eda_3d_conditions.cpp`
 78. `3d-viewer\3d_viewer\tools\eda_3d_controller.cpp`
 79. `3d-viewer\common_ogl\ogl_attr_list.cpp`
 80. `3d-viewer\common_ogl\ogl_utils.cpp`
 81. `3d-viewer\dialogs\appearance_controls_3D.cpp`
 82. `3d-viewer\dialogs\appearance_controls_3D_base.cpp`
 83. `3d-viewer\dialogs\dialog_select_3d_model.cpp`
 84. `3d-viewer\dialogs\dialog_select_3d_model_base.cpp`
 85. `3d-viewer\dialogs\panel_3D_display_options.cpp`
 86. `3d-viewer\dialogs\panel_3D_display_options_base.cpp`
 87. `3d-viewer\dialogs\panel_3D_opengl_options.cpp`
 88. `3d-viewer\dialogs\panel_3D_opengl_options_base.cpp`
 89. `3d-viewer\dialogs\panel_3D_raytracing_options.cpp`
 90. `3d-viewer\dialogs\panel_3D_raytracing_options_base.cpp`
 91. `3d-viewer\dialogs\panel_preview_3d_model.cpp`
 92. `3d-viewer\dialogs\panel_preview_3d_model_base.cpp`
 93. `bitmap2component\bitmap2cmp_control.cpp`
 94. `bitmap2component\bitmap2cmp_frame.cpp`
 95. `bitmap2component\bitmap2cmp_main.cpp`
 96. `bitmap2component\bitmap2cmp_panel.cpp`
 97. `bitmap2component\bitmap2cmp_panel_base.cpp`
 98. `bitmap2component\bitmap2cmp_settings.cpp`
 99. `bitmap2component\bitmap2component.cpp`
100. `build\api\cpp\api\board\board.pb.cc`
101. `build\api\cpp\api\board\board_commands.pb.cc`
102. `build\api\cpp\api\board\board_types.pb.cc`
103. `build\api\cpp\api\common\commands\editor_commands.pb.cc`
104. `build\api\cpp\api\common\commands\project_commands.pb.cc`
105. `build\api\cpp\api\common\envelope.pb.cc`
106. `build\api\cpp\api\common\types\base_types.pb.cc`
107. `build\api\cpp\api\common\types\enums.pb.cc`
108. `build\api\cpp\api\common\types\project_settings.pb.cc`
109. `build\common\gal\glsl_kicad_frag.cpp`
110. `build\common\gal\glsl_kicad_vert.cpp`
111. `build\common\gal\glsl_smaa_base.cpp`
112. `build\common\gal\glsl_smaa_pass_1_frag_luma.cpp`
113. `build\common\gal\glsl_smaa_pass_1_vert.cpp`
114. `build\common\gal\glsl_smaa_pass_2_frag.cpp`
115. `build\common\gal\glsl_smaa_pass_2_vert.cpp`
116. `build\common\gal\glsl_smaa_pass_3_frag.cpp`
117. `build\common\gal\glsl_smaa_pass_3_vert.cpp`
118. `build\common\lib_table_keywords.cpp`
119. `build\common\pcb_keywords.cpp`
120. `build\common\pcb_plot_params_keywords.cpp`
121. `build\eeschema\dialogs\dialog_bom_cfg_keywords.cpp`
122. `build\eeschema\schematic_keywords.cpp`
123. `build\pcbnew\specctra_import_export\specctra_keywords.cpp`
124. `common\advanced_config.cpp`
125. `common\api\api_enums.cpp`
126. `common\api\api_handler.cpp`
127. `common\api\api_handler_common.cpp`
128. `common\api\api_handler_editor.cpp`
129. `common\api\api_plugin.cpp`
130. `common\api\api_plugin_manager.cpp`
131. `common\api\api_server.cpp`
132. `common\api\api_utils.cpp`
133. `common\api\serializable.cpp`
134. `common\array_axis.cpp`
135. `common\array_options.cpp`
136. `common\asset_archive.cpp`
137. `common\background_jobs_monitor.cpp`
138. `common\base_screen.cpp`
139. `common\bin_mod.cpp`
140. `common\bitmap.cpp`
141. `common\bitmap_base.cpp`
142. `common\bitmap_info.cpp`
143. `common\bitmap_store.cpp`
144. `common\board_printout.cpp`
145. `common\build_version.cpp`
146. `common\callback_gal.cpp`
147. `common\clipboard.cpp`
148. `common\commit.cpp`
149. `common\common.cpp`
150. `common\config_params.cpp`
151. `common\confirm.cpp`
152. `common\database\database_connection.cpp`
153. `common\database\database_lib_settings.cpp`
154. `common\design_block_info.cpp`
155. `common\design_block_info_impl.cpp`
156. `common\design_block_io.cpp`
157. `common\design_block_lib_table.cpp`
158. `common\dialog_about\AboutDialog_main.cpp`
159. `common\dialog_about\dialog_about.cpp`
160. `common\dialog_about\dialog_about_base.cpp`
161. `common\dialog_shim.cpp`
162. `common\dialogs\dialog_HTML_reporter_base.cpp`
163. `common\dialogs\dialog_assign_netclass.cpp`
164. `common\dialogs\dialog_assign_netclass_base.cpp`
165. `common\dialogs\dialog_book_reporter.cpp`
166. `common\dialogs\dialog_book_reporter_base.cpp`
167. `common\dialogs\dialog_color_picker.cpp`
168. `common\dialogs\dialog_color_picker_base.cpp`
169. `common\dialogs\dialog_configure_paths.cpp`
170. `common\dialogs\dialog_configure_paths_base.cpp`
171. `common\dialogs\dialog_display_html_text_base.cpp`
172. `common\dialogs\dialog_edit_library_tables.cpp`
173. `common\dialogs\dialog_global_lib_table_config.cpp`
174. `common\dialogs\dialog_global_lib_table_config_base.cpp`
175. `common\dialogs\dialog_grid_settings.cpp`
176. `common\dialogs\dialog_grid_settings_base.cpp`
177. `common\dialogs\dialog_hotkey_list.cpp`
178. `common\dialogs\dialog_import_choose_project.cpp`
179. `common\dialogs\dialog_import_choose_project_base.cpp`
180. `common\dialogs\dialog_locked_items_query.cpp`
181. `common\dialogs\dialog_locked_items_query_base.cpp`
182. `common\dialogs\dialog_migrate_settings.cpp`
183. `common\dialogs\dialog_migrate_settings_base.cpp`
184. `common\dialogs\dialog_multi_unit_entry.cpp`
185. `common\dialogs\dialog_page_settings.cpp`
186. `common\dialogs\dialog_page_settings_base.cpp`
187. `common\dialogs\dialog_paste_special.cpp`
188. `common\dialogs\dialog_paste_special_base.cpp`
189. `common\dialogs\dialog_plugin_options.cpp`
190. `common\dialogs\dialog_plugin_options_base.cpp`
191. `common\dialogs\dialog_print_generic.cpp`
192. `common\dialogs\dialog_print_generic_base.cpp`
193. `common\dialogs\dialog_rc_job.cpp`
194. `common\dialogs\dialog_rc_job_base.cpp`
195. `common\dialogs\dialog_text_entry.cpp`
196. `common\dialogs\dialog_text_entry_base.cpp`
197. `common\dialogs\dialog_unit_entry.cpp`
198. `common\dialogs\dialog_unit_entry_base.cpp`
199. `common\dialogs\eda_list_dialog.cpp`
200. `common\dialogs\eda_list_dialog_base.cpp`
201. `common\dialogs\eda_reorderable_list_dialog.cpp`
202. `common\dialogs\eda_reorderable_list_dialog_base.cpp`
203. `common\dialogs\eda_view_switcher.cpp`
204. `common\dialogs\eda_view_switcher_base.cpp`
205. `common\dialogs\git\panel_git_repos.cpp`
206. `common\dialogs\git\panel_git_repos_base.cpp`
207. `common\dialogs\hotkey_cycle_popup.cpp`
208. `common\dialogs\html_message_box.cpp`
209. `common\dialogs\panel_color_settings.cpp`
210. `common\dialogs\panel_color_settings_base.cpp`
211. `common\dialogs\panel_common_settings.cpp`
212. `common\dialogs\panel_common_settings_base.cpp`
213. `common\dialogs\panel_embedded_files.cpp`
214. `common\dialogs\panel_embedded_files_base.cpp`
215. `common\dialogs\panel_grid_settings.cpp`
216. `common\dialogs\panel_grid_settings_base.cpp`
217. `common\dialogs\panel_hotkeys_editor.cpp`
218. `common\dialogs\panel_image_editor.cpp`
219. `common\dialogs\panel_image_editor_base.cpp`
220. `common\dialogs\panel_mouse_settings.cpp`
221. `common\dialogs\panel_mouse_settings_base.cpp`
222. `common\dialogs\panel_packages_and_updates.cpp`
223. `common\dialogs\panel_packages_and_updates_base.cpp`
224. `common\dialogs\panel_plugin_settings.cpp`
225. `common\dialogs\panel_plugin_settings_base.cpp`
226. `common\dialogs\panel_setup_netclasses.cpp`
227. `common\dialogs\panel_setup_netclasses_base.cpp`
228. `common\dialogs\panel_setup_severities.cpp`
229. `common\dialogs\panel_text_variables.cpp`
230. `common\dialogs\panel_text_variables_base.cpp`
231. `common\draw_panel_gal.cpp`
232. `common\drawing_sheet\drawing_sheet_default_description.cpp`
233. `common\drawing_sheet\drawing_sheet_parser.cpp`
234. `common\drawing_sheet\ds_data_item.cpp`
235. `common\drawing_sheet\ds_data_model.cpp`
236. `common\drawing_sheet\ds_data_model_io.cpp`
237. `common\drawing_sheet\ds_draw_item.cpp`
238. `common\drawing_sheet\ds_painter.cpp`
239. `common\drawing_sheet\ds_proxy_undo_item.cpp`
240. `common\drawing_sheet\ds_proxy_view_item.cpp`
241. `common\dsnlexer.cpp`
242. `common\eda_base_frame.cpp`
243. `common\eda_dde.cpp`
244. `common\eda_doc.cpp`
245. `common\eda_draw_frame.cpp`
246. `common\eda_item.cpp`
247. `common\eda_pattern_match.cpp`
248. `common\eda_shape.cpp`
249. `common\eda_text.cpp`
250. `common\eda_units.cpp`
251. `common\embedded_files.cpp`
252. `common\env_paths.cpp`
253. `common\env_vars.cpp`
254. `common\exceptions.cpp`
255. `common\executable_names.cpp`
256. `common\file_history.cpp`
257. `common\filename_resolver.cpp`
258. `common\filter_reader.cpp`
259. `common\font\font.cpp`
260. `common\font\fontconfig.cpp`
261. `common\font\glyph.cpp`
262. `common\font\outline_decomposer.cpp`
263. `common\font\outline_font.cpp`
264. `common\font\stroke_font.cpp`
265. `common\font\text_attributes.cpp`
266. `common\font\version_info.cpp`
267. `common\footprint_filter.cpp`
268. `common\footprint_info.cpp`
269. `common\fp_lib_table.cpp`
270. `common\gal\3d\camera.cpp`
271. `common\gal\cairo\cairo_compositor.cpp`
272. `common\gal\cairo\cairo_gal.cpp`
273. `common\gal\cairo\cairo_print.cpp`
274. `common\gal\color4d.cpp`
275. `common\gal\cursors.cpp`
276. `common\gal\graphics_abstraction_layer.cpp`
277. `common\gal\hidpi_gl_canvas.cpp`
278. `common\gal\opengl\antialiasing.cpp`
279. `common\gal\opengl\cached_container.cpp`
280. `common\gal\opengl\cached_container_gpu.cpp`
281. `common\gal\opengl\cached_container_ram.cpp`
282. `common\gal\opengl\gl_context_mgr.cpp`
283. `common\gal\opengl\gl_resources.cpp`
284. `common\gal\opengl\gpu_manager.cpp`
285. `common\gal\opengl\noncached_container.cpp`
286. `common\gal\opengl\opengl_compositor.cpp`
287. `common\gal\opengl\opengl_gal.cpp`
288. `common\gal\opengl\shader.cpp`
289. `common\gal\opengl\utils.cpp`
290. `common\gal\opengl\vertex_container.cpp`
291. `common\gal\opengl\vertex_item.cpp`
292. `common\gal\opengl\vertex_manager.cpp`
293. `common\gal\painter.cpp`
294. `common\gal_display_options_common.cpp`
295. `common\gbr_metadata.cpp`
296. `common\gestfich.cpp`
297. `common\gr_basic.cpp`
298. `common\gr_text.cpp`
299. `common\grid_tricks.cpp`
300. `common\hash_eda.cpp`
301. `common\hotkey_store.cpp`
302. `common\hotkeys_basic.cpp`
303. `common\http_lib\http_lib_connection.cpp`
304. `common\http_lib\http_lib_settings.cpp`
305. `common\import_gfx\dxf_import_plugin.cpp`
306. `common\import_gfx\graphics_import_mgr.cpp`
307. `common\import_gfx\graphics_importer.cpp`
308. `common\import_gfx\graphics_importer_buffer.cpp`
309. `common\import_gfx\svg_import_plugin.cpp`
310. `common\increment.cpp`
311. `common\io\altium\altium_ascii_parser.cpp`
312. `common\io\altium\altium_binary_parser.cpp`
313. `common\io\altium\altium_parser_utils.cpp`
314. `common\io\altium\altium_props_utils.cpp`
315. `common\io\cadstar\cadstar_archive_parser.cpp`
316. `common\io\cadstar\cadstar_parts_lib_parser.cpp`
317. `common\io\eagle\eagle_parser.cpp`
318. `common\io\easyeda\easyeda_parser_base.cpp`
319. `common\io\easyeda\easyeda_parser_structs.cpp`
320. `common\io\easyedapro\easyedapro_import_utils.cpp`
321. `common\io\easyedapro\easyedapro_parser.cpp`
322. `common\io\io_base.cpp`
323. `common\io\kicad\kicad_io_utils.cpp`
324. `common\jobs\job.cpp`
325. `common\json_conversions.cpp`
326. `common\json_schema_validator.cpp`
327. `common\kicad_curl\kicad_curl.cpp`
328. `common\kiface_base.cpp`
329. `common\kiid.cpp`
330. `common\kiway.cpp`
331. `common\kiway_express.cpp`
332. `common\kiway_holder.cpp`
333. `common\kiway_player.cpp`
334. `common\launch_ext.cpp`
335. `common\layer_id.cpp`
336. `common\lib_id.cpp`
337. `common\lib_table_base.cpp`
338. `common\lib_table_grid_tricks.cpp`
339. `common\lib_tree_model.cpp`
340. `common\lib_tree_model_adapter.cpp`
341. `common\libeval_compiler\libeval_compiler.cpp`
342. `common\locale_io.cpp`
343. `common\lset.cpp`
344. `common\marker_base.cpp`
345. `common\netclass.cpp`
346. `common\notifications_manager.cpp`
347. `common\origin_transforms.cpp`
348. `common\origin_viewitem.cpp`
349. `common\paths.cpp`
350. `common\pgm_base.cpp`
351. `common\plotters\DXF_plotter.cpp`
352. `common\plotters\GERBER_plotter.cpp`
353. `common\plotters\HPGL_plotter.cpp`
354. `common\plotters\PDF_plotter.cpp`
355. `common\plotters\PS_plotter.cpp`
356. `common\plotters\SVG_plotter.cpp`
357. `common\plotters\common_plot_functions.cpp`
358. `common\plotters\plotter.cpp`
359. `common\preview_items\anchor_debug.cpp`
360. `common\preview_items\arc_assistant.cpp`
361. `common\preview_items\arc_geom_manager.cpp`
362. `common\preview_items\bezier_assistant.cpp`
363. `common\preview_items\bezier_geom_manager.cpp`
364. `common\preview_items\centreline_rect_item.cpp`
365. `common\preview_items\construction_geom.cpp`
366. `common\preview_items\draw_context.cpp`
367. `common\preview_items\item_drawing_utils.cpp`
368. `common\preview_items\polygon_geom_manager.cpp`
369. `common\preview_items\polygon_item.cpp`
370. `common\preview_items\preview_utils.cpp`
371. `common\preview_items\ruler_item.cpp`
372. `common\preview_items\selection_area.cpp`
373. `common\preview_items\simple_overlay_item.cpp`
374. `common\preview_items\snap_indicator.cpp`
375. `common\preview_items\two_point_assistant.cpp`
376. `common\printout.cpp`
377. `common\project.cpp`
378. `common\project\board_project_settings.cpp`
379. `common\project\net_settings.cpp`
380. `common\project\project_archiver.cpp`
381. `common\project\project_file.cpp`
382. `common\project\project_local_settings.cpp`
383. `common\properties\color4d_variant.cpp`
384. `common\properties\eda_angle_variant.cpp`
385. `common\properties\pg_cell_renderer.cpp`
386. `common\properties\pg_editors.cpp`
387. `common\properties\pg_properties.cpp`
388. `common\properties\property_mgr.cpp`
389. `common\properties\std_optional_variants.cpp`
390. `common\ptree.cpp`
391. `common\rc_item.cpp`
392. `common\refdes_utils.cpp`
393. `common\reference_image.cpp`
394. `common\render_settings.cpp`
395. `common\reporter.cpp`
396. `common\richio.cpp`
397. `common\scintilla_tricks.cpp`
398. `common\search_stack.cpp`
399. `common\settings\app_settings.cpp`
400. `common\settings\aui_settings.cpp`
401. `common\settings\bom_settings.cpp`
402. `common\settings\color_settings.cpp`
403. `common\settings\common_settings.cpp`
404. `common\settings\cvpcb_settings.cpp`
405. `common\settings\grid_settings.cpp`
406. `common\settings\json_settings.cpp`
407. `common\settings\kicad_settings.cpp`
408. `common\settings\layer_settings_utils.cpp`
409. `common\settings\nested_settings.cpp`
410. `common\settings\parameters.cpp`
411. `common\settings\settings_manager.cpp`
412. `common\singleton.cpp`
413. `common\status_popup.cpp`
414. `common\string_utils.cpp`
415. `common\stroke_params.cpp`
416. `common\systemdirsappend.cpp`
417. `common\template_fieldnames.cpp`
418. `common\textentry_tricks.cpp`
419. `common\thread_pool.cpp`
420. `common\title_block.cpp`
421. `common\tool\action_manager.cpp`
422. `common\tool\action_menu.cpp`
423. `common\tool\action_toolbar.cpp`
424. `common\tool\actions.cpp`
425. `common\tool\common_control.cpp`
426. `common\tool\common_tools.cpp`
427. `common\tool\conditional_menu.cpp`
428. `common\tool\construction_manager.cpp`
429. `common\tool\edit_constraints.cpp`
430. `common\tool\edit_points.cpp`
431. `common\tool\editor_conditions.cpp`
432. `common\tool\embed_tool.cpp`
433. `common\tool\grid_helper.cpp`
434. `common\tool\grid_menu.cpp`
435. `common\tool\library_editor_control.cpp`
436. `common\tool\picker_tool.cpp`
437. `common\tool\point_editor_behavior.cpp`
438. `common\tool\properties_tool.cpp`
439. `common\tool\selection.cpp`
440. `common\tool\selection_conditions.cpp`
441. `common\tool\selection_tool.cpp`
442. `common\tool\tool_action.cpp`
443. `common\tool\tool_base.cpp`
444. `common\tool\tool_dispatcher.cpp`
445. `common\tool\tool_event.cpp`
446. `common\tool\tool_interactive.cpp`
447. `common\tool\tool_manager.cpp`
448. `common\tool\tool_menu.cpp`
449. `common\tool\tools_holder.cpp`
450. `common\tool\zoom_menu.cpp`
451. `common\tool\zoom_tool.cpp`
452. `common\trace_helpers.cpp`
453. `common\ui_events.cpp`
454. `common\undo_redo_container.cpp`
455. `common\validators.cpp`
456. `common\view\view.cpp`
457. `common\view\view_controls.cpp`
458. `common\view\view_group.cpp`
459. `common\view\view_item.cpp`
460. `common\view\view_overlay.cpp`
461. `common\view\wx_view_controls.cpp`
462. `common\view\zoom_controller.cpp`
463. `common\widgets\app_progress_dialog.cpp`
464. `common\widgets\bitmap_button.cpp`
465. `common\widgets\bitmap_toggle.cpp`
466. `common\widgets\button_row_panel.cpp`
467. `common\widgets\color_swatch.cpp`
468. `common\widgets\filter_combobox.cpp`
469. `common\widgets\font_choice.cpp`
470. `common\widgets\footprint_choice.cpp`
471. `common\widgets\footprint_diff_widget.cpp`
472. `common\widgets\footprint_preview_widget.cpp`
473. `common\widgets\footprint_select_widget.cpp`
474. `common\widgets\gal_options_panel.cpp`
475. `common\widgets\gal_options_panel_base.cpp`
476. `common\widgets\grid_bitmap_toggle.cpp`
477. `common\widgets\grid_checkbox.cpp`
478. `common\widgets\grid_color_swatch_helpers.cpp`
479. `common\widgets\grid_combobox.cpp`
480. `common\widgets\grid_icon_text_helpers.cpp`
481. `common\widgets\grid_text_button_helpers.cpp`
482. `common\widgets\grid_text_helpers.cpp`
483. `common\widgets\html_window.cpp`
484. `common\widgets\indicator_icon.cpp`
485. `common\widgets\kistatusbar.cpp`
486. `common\widgets\layer_box_selector.cpp`
487. `common\widgets\layer_presentation.cpp`
488. `common\widgets\lib_tree.cpp`
489. `common\widgets\listbox_tricks.cpp`
490. `common\widgets\mathplot.cpp`
491. `common\widgets\msgpanel.cpp`
492. `common\widgets\net_selector.cpp`
493. `common\widgets\number_badge.cpp`
494. `common\widgets\paged_dialog.cpp`
495. `common\widgets\progress_reporter_base.cpp`
496. `common\widgets\properties_panel.cpp`
497. `common\widgets\search_pane.cpp`
498. `common\widgets\search_pane_base.cpp`
499. `common\widgets\search_pane_tab.cpp`
500. `common\widgets\split_button.cpp`
501. `common\widgets\std_bitmap_button.cpp`
502. `common\widgets\text_ctrl_eval.cpp`
503. `common\widgets\ui_common.cpp`
504. `common\widgets\unit_binder.cpp`
505. `common\widgets\widget_hotkey_list.cpp`
506. `common\widgets\widget_save_restore.cpp`
507. `common\widgets\wx_aui_art_providers.cpp`
508. `common\widgets\wx_aui_utils.cpp`
509. `common\widgets\wx_busy_indicator.cpp`
510. `common\widgets\wx_collapsible_pane.cpp`
511. `common\widgets\wx_combobox.cpp`
512. `common\widgets\wx_dataviewctrl.cpp`
513. `common\widgets\wx_grid.cpp`
514. `common\widgets\wx_grid_autosizer.cpp`
515. `common\widgets\wx_html_report_box.cpp`
516. `common\widgets\wx_html_report_panel.cpp`
517. `common\widgets\wx_html_report_panel_base.cpp`
518. `common\widgets\wx_infobar.cpp`
519. `common\widgets\wx_listbox.cpp`
520. `common\widgets\wx_panel.cpp`
521. `common\widgets\wx_progress_reporters.cpp`
522. `common\widgets\wx_treebook.cpp`
523. `common\wildcards_and_files_ext.cpp`
524. `common\xnode.cpp`
525. `cvpcb\auto_associate.cpp`
526. `cvpcb\cvpcb.cpp`
527. `cvpcb\cvpcb_mainframe.cpp`
528. `cvpcb\dialogs\dialog_config_equfiles.cpp`
529. `cvpcb\dialogs\dialog_config_equfiles_base.cpp`
530. `cvpcb\dialogs\fp_conflict_assignment_selector.cpp`
531. `cvpcb\dialogs\fp_conflict_assignment_selector_base.cpp`
532. `cvpcb\display_footprints_frame.cpp`
533. `cvpcb\footprints_listbox.cpp`
534. `cvpcb\library_listbox.cpp`
535. `cvpcb\listbox_base.cpp`
536. `cvpcb\menubar.cpp`
537. `cvpcb\readwrite_dlgs.cpp`
538. `cvpcb\symbols_listbox.cpp`
539. `cvpcb\toolbars_cvpcb.cpp`
540. `cvpcb\tools\cvpcb_actions.cpp`
541. `cvpcb\tools\cvpcb_association_tool.cpp`
542. `cvpcb\tools\cvpcb_control.cpp`
543. `cvpcb\tools\cvpcb_fpviewer_selection_tool.cpp`
544. `eeschema\annotate.cpp`
545. `eeschema\api\api_handler_sch.cpp`
546. `eeschema\api\api_sch_utils.cpp`
547. `eeschema\autoplace_fields.cpp`
548. `eeschema\bom_plugins.cpp`
549. `eeschema\bus-wire-junction.cpp`
550. `eeschema\connection_graph.cpp`
551. `eeschema\cross-probing.cpp`
552. `eeschema\design_block_tree_model_adapter.cpp`
553. `eeschema\design_block_utils.cpp`
554. `eeschema\dialogs\dialog_annotate.cpp`
555. `eeschema\dialogs\dialog_annotate_base.cpp`
556. `eeschema\dialogs\dialog_bom.cpp`
557. `eeschema\dialogs\dialog_bom_base.cpp`
558. `eeschema\dialogs\dialog_change_symbols.cpp`
559. `eeschema\dialogs\dialog_change_symbols_base.cpp`
560. `eeschema\dialogs\dialog_database_lib_settings.cpp`
561. `eeschema\dialogs\dialog_database_lib_settings_base.cpp`
562. `eeschema\dialogs\dialog_design_block_properties.cpp`
563. `eeschema\dialogs\dialog_design_block_properties_base.cpp`
564. `eeschema\dialogs\dialog_edit_symbols_libid.cpp`
565. `eeschema\dialogs\dialog_edit_symbols_libid_base.cpp`
566. `eeschema\dialogs\dialog_eeschema_page_settings.cpp`
567. `eeschema\dialogs\dialog_erc.cpp`
568. `eeschema\dialogs\dialog_erc_base.cpp`
569. `eeschema\dialogs\dialog_export_netlist.cpp`
570. `eeschema\dialogs\dialog_export_netlist_base.cpp`
571. `eeschema\dialogs\dialog_field_properties.cpp`
572. `eeschema\dialogs\dialog_field_properties_base.cpp`
573. `eeschema\dialogs\dialog_global_edit_text_and_graphics.cpp`
574. `eeschema\dialogs\dialog_global_edit_text_and_graphics_base.cpp`
575. `eeschema\dialogs\dialog_global_sym_lib_table_config.cpp`
576. `eeschema\dialogs\dialog_image_properties.cpp`
577. `eeschema\dialogs\dialog_image_properties_base.cpp`
578. `eeschema\dialogs\dialog_increment_annotations_base.cpp`
579. `eeschema\dialogs\dialog_junction_props.cpp`
580. `eeschema\dialogs\dialog_junction_props_base.cpp`
581. `eeschema\dialogs\dialog_label_properties.cpp`
582. `eeschema\dialogs\dialog_label_properties_base.cpp`
583. `eeschema\dialogs\dialog_lib_edit_pin_table.cpp`
584. `eeschema\dialogs\dialog_lib_edit_pin_table_base.cpp`
585. `eeschema\dialogs\dialog_lib_new_symbol.cpp`
586. `eeschema\dialogs\dialog_lib_new_symbol_base.cpp`
587. `eeschema\dialogs\dialog_lib_symbol_properties.cpp`
588. `eeschema\dialogs\dialog_lib_symbol_properties_base.cpp`
589. `eeschema\dialogs\dialog_line_properties.cpp`
590. `eeschema\dialogs\dialog_line_properties_base.cpp`
591. `eeschema\dialogs\dialog_migrate_buses.cpp`
592. `eeschema\dialogs\dialog_migrate_buses_base.cpp`
593. `eeschema\dialogs\dialog_pin_properties.cpp`
594. `eeschema\dialogs\dialog_pin_properties_base.cpp`
595. `eeschema\dialogs\dialog_plot_schematic.cpp`
596. `eeschema\dialogs\dialog_plot_schematic_base.cpp`
597. `eeschema\dialogs\dialog_rescue_each.cpp`
598. `eeschema\dialogs\dialog_rescue_each_base.cpp`
599. `eeschema\dialogs\dialog_sch_import_settings.cpp`
600. `eeschema\dialogs\dialog_sch_import_settings_base.cpp`
601. `eeschema\dialogs\dialog_schematic_find.cpp`
602. `eeschema\dialogs\dialog_schematic_find_base.cpp`
603. `eeschema\dialogs\dialog_schematic_setup.cpp`
604. `eeschema\dialogs\dialog_shape_properties.cpp`
605. `eeschema\dialogs\dialog_shape_properties_base.cpp`
606. `eeschema\dialogs\dialog_sheet_pin_properties.cpp`
607. `eeschema\dialogs\dialog_sheet_pin_properties_base.cpp`
608. `eeschema\dialogs\dialog_sheet_properties.cpp`
609. `eeschema\dialogs\dialog_sheet_properties_base.cpp`
610. `eeschema\dialogs\dialog_sim_command.cpp`
611. `eeschema\dialogs\dialog_sim_command_base.cpp`
612. `eeschema\dialogs\dialog_sim_format_value.cpp`
613. `eeschema\dialogs\dialog_sim_format_value_base.cpp`
614. `eeschema\dialogs\dialog_sim_model.cpp`
615. `eeschema\dialogs\dialog_sim_model_base.cpp`
616. `eeschema\dialogs\dialog_symbol_chooser.cpp`
617. `eeschema\dialogs\dialog_symbol_fields_table.cpp`
618. `eeschema\dialogs\dialog_symbol_fields_table_base.cpp`
619. `eeschema\dialogs\dialog_symbol_properties.cpp`
620. `eeschema\dialogs\dialog_symbol_properties_base.cpp`
621. `eeschema\dialogs\dialog_symbol_remap.cpp`
622. `eeschema\dialogs\dialog_symbol_remap_base.cpp`
623. `eeschema\dialogs\dialog_table_properties.cpp`
624. `eeschema\dialogs\dialog_table_properties_base.cpp`
625. `eeschema\dialogs\dialog_tablecell_properties.cpp`
626. `eeschema\dialogs\dialog_tablecell_properties_base.cpp`
627. `eeschema\dialogs\dialog_text_properties.cpp`
628. `eeschema\dialogs\dialog_text_properties_base.cpp`
629. `eeschema\dialogs\dialog_update_from_pcb.cpp`
630. `eeschema\dialogs\dialog_update_from_pcb_base.cpp`
631. `eeschema\dialogs\dialog_update_symbol_fields.cpp`
632. `eeschema\dialogs\dialog_update_symbol_fields_base.cpp`
633. `eeschema\dialogs\dialog_user_defined_signals.cpp`
634. `eeschema\dialogs\dialog_user_defined_signals_base.cpp`
635. `eeschema\dialogs\dialog_wire_bus_properties.cpp`
636. `eeschema\dialogs\dialog_wire_bus_properties_base.cpp`
637. `eeschema\dialogs\panel_bom_presets.cpp`
638. `eeschema\dialogs\panel_bom_presets_base.cpp`
639. `eeschema\dialogs\panel_eeschema_color_settings.cpp`
640. `eeschema\dialogs\panel_setup_buses.cpp`
641. `eeschema\dialogs\panel_setup_buses_base.cpp`
642. `eeschema\dialogs\panel_setup_formatting.cpp`
643. `eeschema\dialogs\panel_setup_formatting_base.cpp`
644. `eeschema\dialogs\panel_setup_pinmap.cpp`
645. `eeschema\dialogs\panel_setup_pinmap_base.cpp`
646. `eeschema\dialogs\panel_sym_lib_table.cpp`
647. `eeschema\dialogs\panel_sym_lib_table_base.cpp`
648. `eeschema\dialogs\panel_template_fieldnames.cpp`
649. `eeschema\dialogs\panel_template_fieldnames_base.cpp`
650. `eeschema\eeschema_config.cpp`
651. `eeschema\eeschema_helpers.cpp`
652. `eeschema\eeschema_settings.cpp`
653. `eeschema\erc\erc.cpp`
654. `eeschema\erc\erc_item.cpp`
655. `eeschema\erc\erc_report.cpp`
656. `eeschema\erc\erc_sch_pin_context.cpp`
657. `eeschema\erc\erc_settings.cpp`
658. `eeschema\fields_data_model.cpp`
659. `eeschema\fields_grid_table.cpp`
660. `eeschema\files-io.cpp`
661. `eeschema\generate_alias_info.cpp`
662. `eeschema\gfx_import_utils.cpp`
663. `eeschema\import_gfx\dialog_import_gfx_sch.cpp`
664. `eeschema\import_gfx\dialog_import_gfx_sch_base.cpp`
665. `eeschema\import_gfx\graphics_importer_lib_symbol.cpp`
666. `eeschema\import_gfx\graphics_importer_sch.cpp`
667. `eeschema\junction_helpers.cpp`
668. `eeschema\lib_symbol.cpp`
669. `eeschema\menubar.cpp`
670. `eeschema\navlib\nl_schematic_plugin.cpp`
671. `eeschema\navlib\nl_schematic_plugin_impl.cpp`
672. `eeschema\net_navigator.cpp`
673. `eeschema\netlist_exporters\netlist_exporter_allegro.cpp`
674. `eeschema\netlist_exporters\netlist_exporter_base.cpp`
675. `eeschema\netlist_exporters\netlist_exporter_cadstar.cpp`
676. `eeschema\netlist_exporters\netlist_exporter_kicad.cpp`
677. `eeschema\netlist_exporters\netlist_exporter_orcadpcb2.cpp`
678. `eeschema\netlist_exporters\netlist_exporter_pads.cpp`
679. `eeschema\netlist_exporters\netlist_exporter_spice.cpp`
680. `eeschema\netlist_exporters\netlist_exporter_spice_model.cpp`
681. `eeschema\netlist_exporters\netlist_exporter_xml.cpp`
682. `eeschema\netlist_exporters\netlist_generator.cpp`
683. `eeschema\picksymbol.cpp`
684. `eeschema\pin_layout_cache.cpp`
685. `eeschema\pin_numbers.cpp`
686. `eeschema\pin_type.cpp`
687. `eeschema\printing\dialog_print.cpp`
688. `eeschema\printing\dialog_print_base.cpp`
689. `eeschema\printing\sch_printout.cpp`
690. `eeschema\project_rescue.cpp`
691. `eeschema\project_sch.cpp`
692. `eeschema\sch_base_frame.cpp`
693. `eeschema\sch_bitmap.cpp`
694. `eeschema\sch_bus_entry.cpp`
695. `eeschema\sch_collectors.cpp`
696. `eeschema\sch_commit.cpp`
697. `eeschema\sch_connection.cpp`
698. `eeschema\sch_draw_panel.cpp`
699. `eeschema\sch_edit_frame.cpp`
700. `eeschema\sch_field.cpp`
701. `eeschema\sch_io\altium\altium_parser_sch.cpp`
702. `eeschema\sch_io\altium\sch_io_altium.cpp`
703. `eeschema\sch_io\cadstar\cadstar_sch_archive_loader.cpp`
704. `eeschema\sch_io\cadstar\cadstar_sch_archive_parser.cpp`
705. `eeschema\sch_io\cadstar\sch_io_cadstar_archive.cpp`
706. `eeschema\sch_io\database\sch_io_database.cpp`
707. `eeschema\sch_io\eagle\sch_io_eagle.cpp`
708. `eeschema\sch_io\easyeda\sch_easyeda_parser.cpp`
709. `eeschema\sch_io\easyeda\sch_io_easyeda.cpp`
710. `eeschema\sch_io\easyedapro\sch_easyedapro_parser.cpp`
711. `eeschema\sch_io\easyedapro\sch_io_easyedapro.cpp`
712. `eeschema\sch_io\http_lib\sch_io_http_lib.cpp`
713. `eeschema\sch_io\kicad_legacy\sch_io_kicad_legacy.cpp`
714. `eeschema\sch_io\kicad_legacy\sch_io_kicad_legacy_helpers.cpp`
715. `eeschema\sch_io\kicad_legacy\sch_io_kicad_legacy_lib_cache.cpp`
716. `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr.cpp`
717. `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr_common.cpp`
718. `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr_lib_cache.cpp`
719. `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr_parser.cpp`
720. `eeschema\sch_io\ltspice\ltspice_schematic.cpp`
721. `eeschema\sch_io\ltspice\sch_io_ltspice.cpp`
722. `eeschema\sch_io\ltspice\sch_io_ltspice_parser.cpp`
723. `eeschema\sch_io\sch_io.cpp`
724. `eeschema\sch_io\sch_io_lib_cache.cpp`
725. `eeschema\sch_io\sch_io_mgr.cpp`
726. `eeschema\sch_item.cpp`
727. `eeschema\sch_junction.cpp`
728. `eeschema\sch_label.cpp`
729. `eeschema\sch_line.cpp`
730. `eeschema\sch_marker.cpp`
731. `eeschema\sch_no_connect.cpp`
732. `eeschema\sch_painter.cpp`
733. `eeschema\sch_pin.cpp`
734. `eeschema\sch_plotter.cpp`
735. `eeschema\sch_preview_panel.cpp`
736. `eeschema\sch_reference_list.cpp`
737. `eeschema\sch_render_settings.cpp`
738. `eeschema\sch_rule_area.cpp`
739. `eeschema\sch_screen.cpp`
740. `eeschema\sch_shape.cpp`
741. `eeschema\sch_sheet.cpp`
742. `eeschema\sch_sheet_path.cpp`
743. `eeschema\sch_sheet_pin.cpp`
744. `eeschema\sch_symbol.cpp`
745. `eeschema\sch_table.cpp`
746. `eeschema\sch_tablecell.cpp`
747. `eeschema\sch_text.cpp`
748. `eeschema\sch_textbox.cpp`
749. `eeschema\sch_validators.cpp`
750. `eeschema\sch_view.cpp`
751. `eeschema\schematic.cpp`
752. `eeschema\schematic_settings.cpp`
753. `eeschema\schematic_undo_redo.cpp`
754. `eeschema\sheet.cpp`
755. `eeschema\sim\kibis\ibis_parser.cpp`
756. `eeschema\sim\kibis\kibis.cpp`
757. `eeschema\sim\legacy_workbook.cpp`
758. `eeschema\sim\ngspice.cpp`
759. `eeschema\sim\sim_lib_mgr.cpp`
760. `eeschema\sim\sim_library.cpp`
761. `eeschema\sim\sim_library_ibis.cpp`
762. `eeschema\sim\sim_library_spice.cpp`
763. `eeschema\sim\sim_model.cpp`
764. `eeschema\sim\sim_model_behavioral.cpp`
765. `eeschema\sim\sim_model_ibis.cpp`
766. `eeschema\sim\sim_model_ideal.cpp`
767. `eeschema\sim\sim_model_l_mutual.cpp`
768. `eeschema\sim\sim_model_ngspice.cpp`
769. `eeschema\sim\sim_model_ngspice_data_b3soi.cpp`
770. `eeschema\sim\sim_model_ngspice_data_b4soi.cpp`
771. `eeschema\sim\sim_model_ngspice_data_bjt.cpp`
772. `eeschema\sim\sim_model_ngspice_data_bsim1.cpp`
773. `eeschema\sim\sim_model_ngspice_data_bsim2.cpp`
774. `eeschema\sim\sim_model_ngspice_data_bsim3.cpp`
775. `eeschema\sim\sim_model_ngspice_data_bsim4.cpp`
776. `eeschema\sim\sim_model_ngspice_data_diode.cpp`
777. `eeschema\sim\sim_model_ngspice_data_hfet.cpp`
778. `eeschema\sim\sim_model_ngspice_data_hicum2.cpp`
779. `eeschema\sim\sim_model_ngspice_data_hsim.cpp`
780. `eeschema\sim\sim_model_ngspice_data_jfet.cpp`
781. `eeschema\sim\sim_model_ngspice_data_mes.cpp`
782. `eeschema\sim\sim_model_ngspice_data_mos.cpp`
783. `eeschema\sim\sim_model_ngspice_data_mos6.cpp`
784. `eeschema\sim\sim_model_ngspice_data_mos9.cpp`
785. `eeschema\sim\sim_model_ngspice_data_vbic.cpp`
786. `eeschema\sim\sim_model_r_pot.cpp`
787. `eeschema\sim\sim_model_raw_spice.cpp`
788. `eeschema\sim\sim_model_serializer.cpp`
789. `eeschema\sim\sim_model_source.cpp`
790. `eeschema\sim\sim_model_spice.cpp`
791. `eeschema\sim\sim_model_spice_fallback.cpp`
792. `eeschema\sim\sim_model_subckt.cpp`
793. `eeschema\sim\sim_model_switch.cpp`
794. `eeschema\sim\sim_model_tline.cpp`
795. `eeschema\sim\sim_model_xspice.cpp`
796. `eeschema\sim\sim_plot_colors.cpp`
797. `eeschema\sim\sim_plot_tab.cpp`
798. `eeschema\sim\sim_property.cpp`
799. `eeschema\sim\sim_tab.cpp`
800. `eeschema\sim\sim_value.cpp`
801. `eeschema\sim\simulator_frame.cpp`
802. `eeschema\sim\simulator_frame_ui.cpp`
803. `eeschema\sim\simulator_frame_ui_base.cpp`
804. `eeschema\sim\spice_circuit_model.cpp`
805. `eeschema\sim\spice_generator.cpp`
806. `eeschema\sim\spice_library_parser.cpp`
807. `eeschema\sim\spice_model_parser.cpp`
808. `eeschema\sim\spice_settings.cpp`
809. `eeschema\sim\spice_simulator.cpp`
810. `eeschema\sim\spice_value.cpp`
811. `eeschema\sim\toolbars_simulator_frame.cpp`
812. `eeschema\symb_transforms_utils.cpp`
813. `eeschema\symbol.cpp`
814. `eeschema\symbol_async_loader.cpp`
815. `eeschema\symbol_checker.cpp`
816. `eeschema\symbol_editor\lib_logger.cpp`
817. `eeschema\symbol_editor\lib_symbol_library_manager.cpp`
818. `eeschema\symbol_editor\menubar_symbol_editor.cpp`
819. `eeschema\symbol_editor\symbol_edit_frame.cpp`
820. `eeschema\symbol_editor\symbol_editor.cpp`
821. `eeschema\symbol_editor\symbol_editor_import_export.cpp`
822. `eeschema\symbol_editor\symbol_editor_plotter.cpp`
823. `eeschema\symbol_editor\symbol_editor_undo_redo.cpp`
824. `eeschema\symbol_editor\toolbars_symbol_editor.cpp`
825. `eeschema\symbol_lib_table.cpp`
826. `eeschema\symbol_library.cpp`
827. `eeschema\symbol_library_manager.cpp`
828. `eeschema\symbol_tree_model_adapter.cpp`
829. `eeschema\symbol_tree_synchronizing_adapter.cpp`
830. `eeschema\symbol_viewer_frame.cpp`
831. `eeschema\sync_sheet_pin\dialog_sync_sheet_pins.cpp`
832. `eeschema\sync_sheet_pin\dialog_sync_sheet_pins_base.cpp`
833. `eeschema\sync_sheet_pin\panel_sync_sheet_pins.cpp`
834. `eeschema\sync_sheet_pin\panel_sync_sheet_pins_base.cpp`
835. `eeschema\sync_sheet_pin\sheet_synchronization_agent.cpp`
836. `eeschema\sync_sheet_pin\sheet_synchronization_item.cpp`
837. `eeschema\sync_sheet_pin\sheet_synchronization_model.cpp`
838. `eeschema\sync_sheet_pin\sheet_synchronization_notifier.cpp`
839. `eeschema\toolbars_sch_editor.cpp`
840. `eeschema\toolbars_symbol_viewer.cpp`
841. `eeschema\tools\assign_footprints.cpp`
842. `eeschema\tools\backannotate.cpp`
843. `eeschema\tools\ee_grid_helper.cpp`
844. `eeschema\tools\rule_area_create_helper.cpp`
845. `eeschema\tools\sch_actions.cpp`
846. `eeschema\tools\sch_design_block_control.cpp`
847. `eeschema\tools\sch_drawing_tools.cpp`
848. `eeschema\tools\sch_edit_table_tool.cpp`
849. `eeschema\tools\sch_edit_tool.cpp`
850. `eeschema\tools\sch_editor_conditions.cpp`
851. `eeschema\tools\sch_editor_control.cpp`
852. `eeschema\tools\sch_find_replace_tool.cpp`
853. `eeschema\tools\sch_inspection_tool.cpp`
854. `eeschema\tools\sch_line_wire_bus_tool.cpp`
855. `eeschema\tools\sch_move_tool.cpp`
856. `eeschema\tools\sch_navigate_tool.cpp`
857. `eeschema\tools\sch_point_editor.cpp`
858. `eeschema\tools\sch_selection.cpp`
859. `eeschema\tools\sch_selection_tool.cpp`
860. `eeschema\tools\sch_tool_utils.cpp`
861. `eeschema\tools\simulator_control.cpp`
862. `eeschema\tools\symbol_editor_control.cpp`
863. `eeschema\tools\symbol_editor_drawing_tools.cpp`
864. `eeschema\tools\symbol_editor_edit_tool.cpp`
865. `eeschema\tools\symbol_editor_move_tool.cpp`
866. `eeschema\tools\symbol_editor_pin_tool.cpp`
867. `eeschema\widgets\design_block_pane.cpp`
868. `eeschema\widgets\design_block_preview_widget.cpp`
869. `eeschema\widgets\hierarchy_pane.cpp`
870. `eeschema\widgets\panel_design_block_chooser.cpp`
871. `eeschema\widgets\panel_sch_selection_filter.cpp`
872. `eeschema\widgets\panel_sch_selection_filter_base.cpp`
873. `eeschema\widgets\panel_symbol_chooser.cpp`
874. `eeschema\widgets\pin_shape_combobox.cpp`
875. `eeschema\widgets\pin_type_combobox.cpp`
876. `eeschema\widgets\sch_properties_panel.cpp`
877. `eeschema\widgets\sch_search_pane.cpp`
878. `eeschema\widgets\search_handlers.cpp`
879. `eeschema\widgets\symbol_diff_widget.cpp`
880. `eeschema\widgets\symbol_filter_combobox.cpp`
881. `eeschema\widgets\symbol_preview_widget.cpp`
882. `eeschema\widgets\symbol_tree_pane.cpp`
883. `eeschema\widgets\tuner_slider.cpp`
884. `eeschema\widgets\tuner_slider_base.cpp`
885. `libs\core\base64.cpp`
886. `libs\core\observable.cpp`
887. `libs\core\profile.cpp`
888. `libs\core\utf8.cpp`
889. `libs\core\version_compare.cpp`
890. `libs\core\wx_stl_compat.cpp`
891. `libs\kimath\src\bezier_curves.cpp`
892. `libs\kimath\src\convert_basic_shapes_to_polygon.cpp`
893. `libs\kimath\src\geometry\circle.cpp`
894. `libs\kimath\src\geometry\convex_hull.cpp`
895. `libs\kimath\src\geometry\corner_operations.cpp`
896. `libs\kimath\src\geometry\direction_45.cpp`
897. `libs\kimath\src\geometry\distribute.cpp`
898. `libs\kimath\src\geometry\eda_angle.cpp`
899. `libs\kimath\src\geometry\ellipse.cpp`
900. `libs\kimath\src\geometry\geometry_utils.cpp`
901. `libs\kimath\src\geometry\half_line.cpp`
902. `libs\kimath\src\geometry\intersection.cpp`
903. `libs\kimath\src\geometry\line.cpp`
904. `libs\kimath\src\geometry\nearest.cpp`
905. `libs\kimath\src\geometry\oval.cpp`
906. `libs\kimath\src\geometry\roundrect.cpp`
907. `libs\kimath\src\geometry\seg.cpp`
908. `libs\kimath\src\geometry\shape.cpp`
909. `libs\kimath\src\geometry\shape_arc.cpp`
910. `libs\kimath\src\geometry\shape_collisions.cpp`
911. `libs\kimath\src\geometry\shape_compound.cpp`
912. `libs\kimath\src\geometry\shape_line_chain.cpp`
913. `libs\kimath\src\geometry\shape_poly_set.cpp`
914. `libs\kimath\src\geometry\shape_rect.cpp`
915. `libs\kimath\src\geometry\shape_segment.cpp`
916. `libs\kimath\src\geometry\shape_utils.cpp`
917. `libs\kimath\src\geometry\vector_utils.cpp`
918. `libs\kimath\src\geometry\vertex_set.cpp`
919. `libs\kimath\src\math\util.cpp`
920. `libs\kimath\src\math\vector2.cpp`
921. `libs\kimath\src\transform.cpp`
922. `libs\kimath\src\trigo.cpp`
923. `libs\kinng\src\kinng.cpp`
924. `libs\kiplatform\os\windows\app.cpp`
925. `libs\kiplatform\os\windows\drivers.cpp`
926. `libs\kiplatform\os\windows\environment.cpp`
927. `libs\kiplatform\os\windows\io.cpp`
928. `libs\kiplatform\port\wxmsw\ui.cpp`
929. `libs\sexpr\sexpr.cpp`
930. `libs\sexpr\sexpr_parser.cpp`
931. `pcbnew\action_plugin.cpp`
932. `pcbnew\api\api_handler_pcb.cpp`
933. `pcbnew\api\api_pcb_enums.cpp`
934. `pcbnew\api\api_pcb_utils.cpp`
935. `pcbnew\array_pad_number_provider.cpp`
936. `pcbnew\autorouter\ar_autoplacer.cpp`
937. `pcbnew\autorouter\ar_matrix.cpp`
938. `pcbnew\autorouter\autoplace_tool.cpp`
939. `pcbnew\autorouter\spread_footprints.cpp`
940. `pcbnew\board.cpp`
941. `pcbnew\board_commit.cpp`
942. `pcbnew\board_connected_item.cpp`
943. `pcbnew\board_design_settings.cpp`
944. `pcbnew\board_item.cpp`
945. `pcbnew\board_stackup_manager\board_stackup.cpp`
946. `pcbnew\board_stackup_manager\board_stackup_reporter.cpp`
947. `pcbnew\board_stackup_manager\dialog_dielectric_list_manager.cpp`
948. `pcbnew\board_stackup_manager\dialog_dielectric_list_manager_base.cpp`
949. `pcbnew\board_stackup_manager\dielectric_material.cpp`
950. `pcbnew\board_stackup_manager\panel_board_finish.cpp`
951. `pcbnew\board_stackup_manager\panel_board_finish_base.cpp`
952. `pcbnew\board_stackup_manager\panel_board_stackup.cpp`
953. `pcbnew\board_stackup_manager\panel_board_stackup_base.cpp`
954. `pcbnew\board_stackup_manager\stackup_predefined_prms.cpp`
955. `pcbnew\build_BOM_from_board.cpp`
956. `pcbnew\cleanup_item.cpp`
957. `pcbnew\collectors.cpp`
958. `pcbnew\component_class_manager.cpp`
959. `pcbnew\connectivity\connectivity_algo.cpp`
960. `pcbnew\connectivity\connectivity_data.cpp`
961. `pcbnew\connectivity\connectivity_items.cpp`
962. `pcbnew\connectivity\from_to_cache.cpp`
963. `pcbnew\connectivity\topo_match.cpp`
964. `pcbnew\convert_shape_list_to_polygon.cpp`
965. `pcbnew\cross-probing.cpp`
966. `pcbnew\dialogs\dialog_board_reannotate.cpp`
967. `pcbnew\dialogs\dialog_board_reannotate_base.cpp`
968. `pcbnew\dialogs\dialog_board_setup.cpp`
969. `pcbnew\dialogs\dialog_board_statistics.cpp`
970. `pcbnew\dialogs\dialog_board_statistics_base.cpp`
971. `pcbnew\dialogs\dialog_cleanup_graphics.cpp`
972. `pcbnew\dialogs\dialog_cleanup_graphics_base.cpp`
973. `pcbnew\dialogs\dialog_cleanup_tracks_and_vias.cpp`
974. `pcbnew\dialogs\dialog_cleanup_tracks_and_vias_base.cpp`
975. `pcbnew\dialogs\dialog_copper_zones.cpp`
976. `pcbnew\dialogs\dialog_copper_zones_base.cpp`
977. `pcbnew\dialogs\dialog_create_array.cpp`
978. `pcbnew\dialogs\dialog_create_array_base.cpp`
979. `pcbnew\dialogs\dialog_dimension_properties.cpp`
980. `pcbnew\dialogs\dialog_dimension_properties_base.cpp`
981. `pcbnew\dialogs\dialog_drc.cpp`
982. `pcbnew\dialogs\dialog_drc_base.cpp`
983. `pcbnew\dialogs\dialog_drc_job_config.cpp`
984. `pcbnew\dialogs\dialog_enum_pads.cpp`
985. `pcbnew\dialogs\dialog_enum_pads_base.cpp`
986. `pcbnew\dialogs\dialog_exchange_footprints.cpp`
987. `pcbnew\dialogs\dialog_exchange_footprints_base.cpp`
988. `pcbnew\dialogs\dialog_export_2581.cpp`
989. `pcbnew\dialogs\dialog_export_2581_base.cpp`
990. `pcbnew\dialogs\dialog_export_idf.cpp`
991. `pcbnew\dialogs\dialog_export_idf_base.cpp`
992. `pcbnew\dialogs\dialog_export_odbpp.cpp`
993. `pcbnew\dialogs\dialog_export_odbpp_base.cpp`
994. `pcbnew\dialogs\dialog_export_step.cpp`
995. `pcbnew\dialogs\dialog_export_step_base.cpp`
996. `pcbnew\dialogs\dialog_export_step_process.cpp`
997. `pcbnew\dialogs\dialog_export_step_process_base.cpp`
998. `pcbnew\dialogs\dialog_export_vrml.cpp`
999. `pcbnew\dialogs\dialog_export_vrml_base.cpp`
1000. `pcbnew\dialogs\dialog_filter_selection.cpp`
1001. `pcbnew\dialogs\dialog_filter_selection_base.cpp`
1002. `pcbnew\dialogs\dialog_find.cpp`
1003. `pcbnew\dialogs\dialog_find_base.cpp`
1004. `pcbnew\dialogs\dialog_footprint_associations.cpp`
1005. `pcbnew\dialogs\dialog_footprint_associations_base.cpp`
1006. `pcbnew\dialogs\dialog_footprint_checker.cpp`
1007. `pcbnew\dialogs\dialog_footprint_checker_base.cpp`
1008. `pcbnew\dialogs\dialog_footprint_properties.cpp`
1009. `pcbnew\dialogs\dialog_footprint_properties_base.cpp`
1010. `pcbnew\dialogs\dialog_footprint_properties_fp_editor.cpp`
1011. `pcbnew\dialogs\dialog_footprint_properties_fp_editor_base.cpp`
1012. `pcbnew\dialogs\dialog_footprint_wizard_list.cpp`
1013. `pcbnew\dialogs\dialog_footprint_wizard_list_base.cpp`
1014. `pcbnew\dialogs\dialog_gen_footprint_position.cpp`
1015. `pcbnew\dialogs\dialog_gen_footprint_position_file_base.cpp`
1016. `pcbnew\dialogs\dialog_gencad_export_options.cpp`
1017. `pcbnew\dialogs\dialog_gendrill.cpp`
1018. `pcbnew\dialogs\dialog_gendrill_base.cpp`
1019. `pcbnew\dialogs\dialog_generators.cpp`
1020. `pcbnew\dialogs\dialog_generators_base.cpp`
1021. `pcbnew\dialogs\dialog_get_footprint_by_name.cpp`
1022. `pcbnew\dialogs\dialog_get_footprint_by_name_base.cpp`
1023. `pcbnew\dialogs\dialog_global_deletion.cpp`
1024. `pcbnew\dialogs\dialog_global_deletion_base.cpp`
1025. `pcbnew\dialogs\dialog_global_edit_teardrops.cpp`
1026. `pcbnew\dialogs\dialog_global_edit_teardrops_base.cpp`
1027. `pcbnew\dialogs\dialog_global_edit_text_and_graphics.cpp`
1028. `pcbnew\dialogs\dialog_global_edit_text_and_graphics_base.cpp`
1029. `pcbnew\dialogs\dialog_global_edit_tracks_and_vias.cpp`
1030. `pcbnew\dialogs\dialog_global_edit_tracks_and_vias_base.cpp`
1031. `pcbnew\dialogs\dialog_global_fp_lib_table_config.cpp`
1032. `pcbnew\dialogs\dialog_group_properties.cpp`
1033. `pcbnew\dialogs\dialog_group_properties_base.cpp`
1034. `pcbnew\dialogs\dialog_import_netlist.cpp`
1035. `pcbnew\dialogs\dialog_import_netlist_base.cpp`
1036. `pcbnew\dialogs\dialog_import_settings.cpp`
1037. `pcbnew\dialogs\dialog_import_settings_base.cpp`
1038. `pcbnew\dialogs\dialog_imported_layers_base.cpp`
1039. `pcbnew\dialogs\dialog_layer_selection_base.cpp`
1040. `pcbnew\dialogs\dialog_map_layers.cpp`
1041. `pcbnew\dialogs\dialog_move_exact.cpp`
1042. `pcbnew\dialogs\dialog_move_exact_base.cpp`
1043. `pcbnew\dialogs\dialog_multichannel_generate_rule_areas.cpp`
1044. `pcbnew\dialogs\dialog_multichannel_generate_rule_areas_base.cpp`
1045. `pcbnew\dialogs\dialog_multichannel_repeat_layout.cpp`
1046. `pcbnew\dialogs\dialog_multichannel_repeat_layout_base.cpp`
1047. `pcbnew\dialogs\dialog_non_copper_zones_properties.cpp`
1048. `pcbnew\dialogs\dialog_non_copper_zones_properties_base.cpp`
1049. `pcbnew\dialogs\dialog_outset_items.cpp`
1050. `pcbnew\dialogs\dialog_outset_items_base.cpp`
1051. `pcbnew\dialogs\dialog_pad_properties.cpp`
1052. `pcbnew\dialogs\dialog_pad_properties_base.cpp`
1053. `pcbnew\dialogs\dialog_plot.cpp`
1054. `pcbnew\dialogs\dialog_plot_base.cpp`
1055. `pcbnew\dialogs\dialog_pns_diff_pair_dimensions.cpp`
1056. `pcbnew\dialogs\dialog_pns_diff_pair_dimensions_base.cpp`
1057. `pcbnew\dialogs\dialog_pns_settings.cpp`
1058. `pcbnew\dialogs\dialog_pns_settings_base.cpp`
1059. `pcbnew\dialogs\dialog_position_relative.cpp`
1060. `pcbnew\dialogs\dialog_position_relative_base.cpp`
1061. `pcbnew\dialogs\dialog_print_pcbnew.cpp`
1062. `pcbnew\dialogs\dialog_push_pad_properties.cpp`
1063. `pcbnew\dialogs\dialog_push_pad_properties_base.cpp`
1064. `pcbnew\dialogs\dialog_reference_image_properties.cpp`
1065. `pcbnew\dialogs\dialog_reference_image_properties_base.cpp`
1066. `pcbnew\dialogs\dialog_render_job.cpp`
1067. `pcbnew\dialogs\dialog_render_job_base.cpp`
1068. `pcbnew\dialogs\dialog_rule_area_properties.cpp`
1069. `pcbnew\dialogs\dialog_rule_area_properties_base.cpp`
1070. `pcbnew\dialogs\dialog_set_offset.cpp`
1071. `pcbnew\dialogs\dialog_set_offset_base.cpp`
1072. `pcbnew\dialogs\dialog_shape_properties.cpp`
1073. `pcbnew\dialogs\dialog_shape_properties_base.cpp`
1074. `pcbnew\dialogs\dialog_swap_layers.cpp`
1075. `pcbnew\dialogs\dialog_swap_layers_base.cpp`
1076. `pcbnew\dialogs\dialog_table_properties.cpp`
1077. `pcbnew\dialogs\dialog_table_properties_base.cpp`
1078. `pcbnew\dialogs\dialog_tablecell_properties.cpp`
1079. `pcbnew\dialogs\dialog_target_properties.cpp`
1080. `pcbnew\dialogs\dialog_target_properties_base.cpp`
1081. `pcbnew\dialogs\dialog_text_properties.cpp`
1082. `pcbnew\dialogs\dialog_text_properties_base.cpp`
1083. `pcbnew\dialogs\dialog_textbox_properties.cpp`
1084. `pcbnew\dialogs\dialog_textbox_properties_base.cpp`
1085. `pcbnew\dialogs\dialog_track_via_properties.cpp`
1086. `pcbnew\dialogs\dialog_track_via_properties_base.cpp`
1087. `pcbnew\dialogs\dialog_track_via_size.cpp`
1088. `pcbnew\dialogs\dialog_track_via_size_base.cpp`
1089. `pcbnew\dialogs\dialog_tuning_pattern_properties.cpp`
1090. `pcbnew\dialogs\dialog_tuning_pattern_properties_base.cpp`
1091. `pcbnew\dialogs\dialog_unused_pad_layers.cpp`
1092. `pcbnew\dialogs\dialog_unused_pad_layers_base.cpp`
1093. `pcbnew\dialogs\dialog_update_pcb.cpp`
1094. `pcbnew\dialogs\dialog_update_pcb_base.cpp`
1095. `pcbnew\dialogs\panel_edit_options.cpp`
1096. `pcbnew\dialogs\panel_edit_options_base.cpp`
1097. `pcbnew\dialogs\panel_fp_editor_color_settings.cpp`
1098. `pcbnew\dialogs\panel_fp_editor_field_defaults.cpp`
1099. `pcbnew\dialogs\panel_fp_editor_field_defaults_base.cpp`
1100. `pcbnew\dialogs\panel_fp_editor_graphics_defaults.cpp`
1101. `pcbnew\dialogs\panel_fp_editor_graphics_defaults_base.cpp`
1102. `pcbnew\dialogs\panel_fp_lib_table.cpp`
1103. `pcbnew\dialogs\panel_fp_lib_table_base.cpp`
1104. `pcbnew\dialogs\panel_fp_properties_3d_model.cpp`
1105. `pcbnew\dialogs\panel_fp_properties_3d_model_base.cpp`
1106. `pcbnew\dialogs\panel_pcb_display_options.cpp`
1107. `pcbnew\dialogs\panel_pcb_display_options_base.cpp`
1108. `pcbnew\dialogs\panel_pcbnew_action_plugins.cpp`
1109. `pcbnew\dialogs\panel_pcbnew_action_plugins_base.cpp`
1110. `pcbnew\dialogs\panel_pcbnew_color_settings.cpp`
1111. `pcbnew\dialogs\panel_pcbnew_display_origin.cpp`
1112. `pcbnew\dialogs\panel_pcbnew_display_origin_base.cpp`
1113. `pcbnew\dialogs\panel_rule_area_properties_keepout_base.cpp`
1114. `pcbnew\dialogs\panel_rule_area_properties_placement_base.cpp`
1115. `pcbnew\dialogs\panel_setup_constraints.cpp`
1116. `pcbnew\dialogs\panel_setup_constraints_base.cpp`
1117. `pcbnew\dialogs\panel_setup_dimensions.cpp`
1118. `pcbnew\dialogs\panel_setup_dimensions_base.cpp`
1119. `pcbnew\dialogs\panel_setup_formatting.cpp`
1120. `pcbnew\dialogs\panel_setup_formatting_base.cpp`
1121. `pcbnew\dialogs\panel_setup_layers.cpp`
1122. `pcbnew\dialogs\panel_setup_layers_base.cpp`
1123. `pcbnew\dialogs\panel_setup_mask_and_paste.cpp`
1124. `pcbnew\dialogs\panel_setup_mask_and_paste_base.cpp`
1125. `pcbnew\dialogs\panel_setup_rules.cpp`
1126. `pcbnew\dialogs\panel_setup_rules_base.cpp`
1127. `pcbnew\dialogs\panel_setup_teardrops.cpp`
1128. `pcbnew\dialogs\panel_setup_teardrops_base.cpp`
1129. `pcbnew\dialogs\panel_setup_text_and_graphics.cpp`
1130. `pcbnew\dialogs\panel_setup_text_and_graphics_base.cpp`
1131. `pcbnew\dialogs\panel_setup_tracks_and_vias.cpp`
1132. `pcbnew\dialogs\panel_setup_tracks_and_vias_base.cpp`
1133. `pcbnew\dialogs\panel_setup_tuning_patterns.cpp`
1134. `pcbnew\dialogs\panel_setup_tuning_patterns_base.cpp`
1135. `pcbnew\drc\drc_cache_generator.cpp`
1136. `pcbnew\drc\drc_engine.cpp`
1137. `pcbnew\drc\drc_interactive_courtyard_clearance.cpp`
1138. `pcbnew\drc\drc_item.cpp`
1139. `pcbnew\drc\drc_report.cpp`
1140. `pcbnew\drc\drc_rule.cpp`
1141. `pcbnew\drc\drc_rule_condition.cpp`
1142. `pcbnew\drc\drc_rule_parser.cpp`
1143. `pcbnew\drc\drc_test_provider.cpp`
1144. `pcbnew\drc\drc_test_provider_library_parity.cpp`
1145. `pcbnew\edit.cpp`
1146. `pcbnew\edit_track_width.cpp`
1147. `pcbnew\edit_zone_helpers.cpp`
1148. `pcbnew\exporters\export_d356.cpp`
1149. `pcbnew\exporters\export_footprint_associations.cpp`
1150. `pcbnew\exporters\export_gencad.cpp`
1151. `pcbnew\exporters\export_gencad_writer.cpp`
1152. `pcbnew\exporters\export_hyperlynx.cpp`
1153. `pcbnew\exporters\export_idf.cpp`
1154. `pcbnew\exporters\export_svg.cpp`
1155. `pcbnew\exporters\exporter_vrml.cpp`
1156. `pcbnew\exporters\gen_drill_report_files.cpp`
1157. `pcbnew\exporters\gendrill_Excellon_writer.cpp`
1158. `pcbnew\exporters\gendrill_file_writer_base.cpp`
1159. `pcbnew\exporters\gendrill_gerber_writer.cpp`
1160. `pcbnew\exporters\gerber_jobfile_writer.cpp`
1161. `pcbnew\exporters\gerber_placefile_writer.cpp`
1162. `pcbnew\exporters\place_file_exporter.cpp`
1163. `pcbnew\exporters\step\KI_XCAFDoc_AssemblyGraph.cxx`
1164. `pcbnew\exporters\step\exporter_step.cpp`
1165. `pcbnew\exporters\step\step_pcb_model.cpp`
1166. `pcbnew\files.cpp`
1167. `pcbnew\fix_board_shape.cpp`
1168. `pcbnew\footprint.cpp`
1169. `pcbnew\footprint_chooser_frame.cpp`
1170. `pcbnew\footprint_edit_frame.cpp`
1171. `pcbnew\footprint_editor_settings.cpp`
1172. `pcbnew\footprint_editor_utils.cpp`
1173. `pcbnew\footprint_info_impl.cpp`
1174. `pcbnew\footprint_libraries_utils.cpp`
1175. `pcbnew\footprint_preview_panel.cpp`
1176. `pcbnew\footprint_tree_pane.cpp`
1177. `pcbnew\footprint_viewer_frame.cpp`
1178. `pcbnew\footprint_wizard.cpp`
1179. `pcbnew\footprint_wizard_frame.cpp`
1180. `pcbnew\footprint_wizard_frame_functions.cpp`
1181. `pcbnew\fp_tree_model_adapter.cpp`
1182. `pcbnew\fp_tree_synchronizing_adapter.cpp`
1183. `pcbnew\generate_footprint_info.cpp`
1184. `pcbnew\generators\pcb_tuning_pattern.cpp`
1185. `pcbnew\generators_mgr.cpp`
1186. `pcbnew\graphics_cleaner.cpp`
1187. `pcbnew\grid_layer_box_helpers.cpp`
1188. `pcbnew\import_gfx\dialog_import_graphics.cpp`
1189. `pcbnew\import_gfx\dialog_import_graphics_base.cpp`
1190. `pcbnew\import_gfx\graphics_importer_pcbnew.cpp`
1191. `pcbnew\initpcb.cpp`
1192. `pcbnew\kicad_clipboard.cpp`
1193. `pcbnew\layer_pairs.cpp`
1194. `pcbnew\load_select_footprint.cpp`
1195. `pcbnew\menubar_footprint_editor.cpp`
1196. `pcbnew\menubar_pcb_editor.cpp`
1197. `pcbnew\microwave\microwave_footprint.cpp`
1198. `pcbnew\microwave\microwave_inductor.cpp`
1199. `pcbnew\microwave\microwave_polygon.cpp`
1200. `pcbnew\microwave\microwave_tool.cpp`
1201. `pcbnew\navlib\nl_pcbnew_plugin.cpp`
1202. `pcbnew\navlib\nl_pcbnew_plugin_impl.cpp`
1203. `pcbnew\netinfo_item.cpp`
1204. `pcbnew\netinfo_list.cpp`
1205. `pcbnew\netlist_reader\board_netlist_updater.cpp`
1206. `pcbnew\netlist_reader\kicad_netlist_reader.cpp`
1207. `pcbnew\netlist_reader\legacy_netlist_reader.cpp`
1208. `pcbnew\netlist_reader\netlist.cpp`
1209. `pcbnew\netlist_reader\netlist_reader.cpp`
1210. `pcbnew\netlist_reader\pcb_netlist.cpp`
1211. `pcbnew\pad.cpp`
1212. `pcbnew\pad_utils.cpp`
1213. `pcbnew\padstack.cpp`
1214. `pcbnew\pcb_base_edit_frame.cpp`
1215. `pcbnew\pcb_base_frame.cpp`
1216. `pcbnew\pcb_dimension.cpp`
1217. `pcbnew\pcb_draw_panel_gal.cpp`
1218. `pcbnew\pcb_edit_frame.cpp`
1219. `pcbnew\pcb_field.cpp`
1220. `pcbnew\pcb_fields_grid_table.cpp`
1221. `pcbnew\pcb_generator.cpp`
1222. `pcbnew\pcb_group.cpp`
1223. `pcbnew\pcb_io\altium\altium_parser_pcb.cpp`
1224. `pcbnew\pcb_io\altium\altium_pcb.cpp`
1225. `pcbnew\pcb_io\altium\altium_pcb_compound_file.cpp`
1226. `pcbnew\pcb_io\altium\pcb_io_altium_circuit_maker.cpp`
1227. `pcbnew\pcb_io\altium\pcb_io_altium_circuit_studio.cpp`
1228. `pcbnew\pcb_io\altium\pcb_io_altium_designer.cpp`
1229. `pcbnew\pcb_io\altium\pcb_io_solidworks.cpp`
1230. `pcbnew\pcb_io\cadstar\cadstar_pcb_archive_loader.cpp`
1231. `pcbnew\pcb_io\cadstar\cadstar_pcb_archive_parser.cpp`
1232. `pcbnew\pcb_io\cadstar\pcb_io_cadstar_archive.cpp`
1233. `pcbnew\pcb_io\eagle\pcb_io_eagle.cpp`
1234. `pcbnew\pcb_io\easyeda\pcb_io_easyeda_parser.cpp`
1235. `pcbnew\pcb_io\easyeda\pcb_io_easyeda_plugin.cpp`
1236. `pcbnew\pcb_io\easyedapro\pcb_io_easyedapro.cpp`
1237. `pcbnew\pcb_io\easyedapro\pcb_io_easyedapro_parser.cpp`
1238. `pcbnew\pcb_io\fabmaster\import_fabmaster.cpp`
1239. `pcbnew\pcb_io\fabmaster\pcb_io_fabmaster.cpp`
1240. `pcbnew\pcb_io\geda\pcb_io_geda.cpp`
1241. `pcbnew\pcb_io\ipc2581\pcb_io_ipc2581.cpp`
1242. `pcbnew\pcb_io\kicad_legacy\pcb_io_kicad_legacy.cpp`
1243. `pcbnew\pcb_io\kicad_sexpr\pcb_io_kicad_sexpr.cpp`
1244. `pcbnew\pcb_io\kicad_sexpr\pcb_io_kicad_sexpr_parser.cpp`
1245. `pcbnew\pcb_io\odbpp\odb_attribute.cpp`
1246. `pcbnew\pcb_io\odbpp\odb_component.cpp`
1247. `pcbnew\pcb_io\odbpp\odb_eda_data.cpp`
1248. `pcbnew\pcb_io\odbpp\odb_entity.cpp`
1249. `pcbnew\pcb_io\odbpp\odb_feature.cpp`
1250. `pcbnew\pcb_io\odbpp\odb_fonts.cpp`
1251. `pcbnew\pcb_io\odbpp\odb_netlist.cpp`
1252. `pcbnew\pcb_io\odbpp\odb_util.cpp`
1253. `pcbnew\pcb_io\odbpp\pcb_io_odbpp.cpp`
1254. `pcbnew\pcb_io\pcad\pcad2kicad_common.cpp`
1255. `pcbnew\pcb_io\pcad\pcad_arc.cpp`
1256. `pcbnew\pcb_io\pcad\pcad_copper_pour.cpp`
1257. `pcbnew\pcb_io\pcad\pcad_cutout.cpp`
1258. `pcbnew\pcb_io\pcad\pcad_footprint.cpp`
1259. `pcbnew\pcb_io\pcad\pcad_keepout.cpp`
1260. `pcbnew\pcb_io\pcad\pcad_line.cpp`
1261. `pcbnew\pcb_io\pcad\pcad_nets.cpp`
1262. `pcbnew\pcb_io\pcad\pcad_pad.cpp`
1263. `pcbnew\pcb_io\pcad\pcad_pad_shape.cpp`
1264. `pcbnew\pcb_io\pcad\pcad_pcb.cpp`
1265. `pcbnew\pcb_io\pcad\pcad_pcb_component.cpp`
1266. `pcbnew\pcb_io\pcad\pcad_plane.cpp`
1267. `pcbnew\pcb_io\pcad\pcad_polygon.cpp`
1268. `pcbnew\pcb_io\pcad\pcad_text.cpp`
1269. `pcbnew\pcb_io\pcad\pcad_via.cpp`
1270. `pcbnew\pcb_io\pcad\pcad_via_shape.cpp`
1271. `pcbnew\pcb_io\pcad\pcb_io_pcad.cpp`
1272. `pcbnew\pcb_io\pcad\s_expr_loader.cpp`
1273. `pcbnew\pcb_io\pcb_io.cpp`
1274. `pcbnew\pcb_io\pcb_io_mgr.cpp`
1275. `pcbnew\pcb_layer_box_selector.cpp`
1276. `pcbnew\pcb_marker.cpp`
1277. `pcbnew\pcb_origin_transforms.cpp`
1278. `pcbnew\pcb_painter.cpp`
1279. `pcbnew\pcb_plot_params.cpp`
1280. `pcbnew\pcb_plotter.cpp`
1281. `pcbnew\pcb_reference_image.cpp`
1282. `pcbnew\pcb_screen.cpp`
1283. `pcbnew\pcb_shape.cpp`
1284. `pcbnew\pcb_table.cpp`
1285. `pcbnew\pcb_tablecell.cpp`
1286. `pcbnew\pcb_target.cpp`
1287. `pcbnew\pcb_text.cpp`
1288. `pcbnew\pcb_textbox.cpp`
1289. `pcbnew\pcb_track.cpp`
1290. `pcbnew\pcb_view.cpp`
1291. `pcbnew\pcbexpr_evaluator.cpp`
1292. `pcbnew\pcbexpr_functions.cpp`
1293. `pcbnew\pcbnew.cpp`
1294. `pcbnew\pcbnew_config.cpp`
1295. `pcbnew\pcbnew_jobs_handler.cpp`
1296. `pcbnew\pcbnew_printout.cpp`
1297. `pcbnew\pcbnew_settings.cpp`
1298. `pcbnew\pcbplot.cpp`
1299. `pcbnew\plot_board_layers.cpp`
1300. `pcbnew\plot_brditems_plotter.cpp`
1301. `pcbnew\project_pcb.cpp`
1302. `pcbnew\python\scripting\pcb_scripting_tool.cpp`
1303. `pcbnew\python\scripting\pcbnew_action_plugins.cpp`
1304. `pcbnew\python\scripting\pcbnew_scripting.cpp`
1305. `pcbnew\python\scripting\pcbnew_scripting_helpers.cpp`
1306. `pcbnew\ratsnest\ratsnest.cpp`
1307. `pcbnew\ratsnest\ratsnest_data.cpp`
1308. `pcbnew\ratsnest\ratsnest_view_item.cpp`
1309. `pcbnew\router\pns_algo_base.cpp`
1310. `pcbnew\router\pns_arc.cpp`
1311. `pcbnew\router\pns_component_dragger.cpp`
1312. `pcbnew\router\pns_diff_pair.cpp`
1313. `pcbnew\router\pns_diff_pair_placer.cpp`
1314. `pcbnew\router\pns_dp_meander_placer.cpp`
1315. `pcbnew\router\pns_dragger.cpp`
1316. `pcbnew\router\pns_hole.cpp`
1317. `pcbnew\router\pns_index.cpp`
1318. `pcbnew\router\pns_item.cpp`
1319. `pcbnew\router\pns_itemset.cpp`
1320. `pcbnew\router\pns_kicad_iface.cpp`
1321. `pcbnew\router\pns_line.cpp`
1322. `pcbnew\router\pns_line_placer.cpp`
1323. `pcbnew\router\pns_logger.cpp`
1324. `pcbnew\router\pns_meander.cpp`
1325. `pcbnew\router\pns_meander_placer.cpp`
1326. `pcbnew\router\pns_meander_placer_base.cpp`
1327. `pcbnew\router\pns_meander_skew_placer.cpp`
1328. `pcbnew\router\pns_mouse_trail_tracer.cpp`
1329. `pcbnew\router\pns_multi_dragger.cpp`
1330. `pcbnew\router\pns_node.cpp`
1331. `pcbnew\router\pns_optimizer.cpp`
1332. `pcbnew\router\pns_router.cpp`
1333. `pcbnew\router\pns_routing_settings.cpp`
1334. `pcbnew\router\pns_shove.cpp`
1335. `pcbnew\router\pns_sizes_settings.cpp`
1336. `pcbnew\router\pns_solid.cpp`
1337. `pcbnew\router\pns_tool_base.cpp`
1338. `pcbnew\router\pns_topology.cpp`
1339. `pcbnew\router\pns_utils.cpp`
1340. `pcbnew\router\pns_via.cpp`
1341. `pcbnew\router\pns_walkaround.cpp`
1342. `pcbnew\router\router_preview_item.cpp`
1343. `pcbnew\router\router_status_view_item.cpp`
1344. `pcbnew\router\router_tool.cpp`
1345. `pcbnew\router\time_limit.cpp`
1346. `pcbnew\sel_layer.cpp`
1347. `pcbnew\specctra_import_export\specctra.cpp`
1348. `pcbnew\specctra_import_export\specctra_export.cpp`
1349. `pcbnew\specctra_import_export\specctra_import.cpp`
1350. `pcbnew\teardrop\teardrop.cpp`
1351. `pcbnew\teardrop\teardrop_parameters.cpp`
1352. `pcbnew\teardrop\teardrop_utils.cpp`
1353. `pcbnew\toolbars_footprint_editor.cpp`
1354. `pcbnew\toolbars_footprint_viewer.cpp`
1355. `pcbnew\toolbars_pcb_editor.cpp`
1356. `pcbnew\tools\align_distribute_tool.cpp`
1357. `pcbnew\tools\array_tool.cpp`
1358. `pcbnew\tools\board_editor_control.cpp`
1359. `pcbnew\tools\board_inspection_tool.cpp`
1360. `pcbnew\tools\board_reannotate_tool.cpp`
1361. `pcbnew\tools\convert_tool.cpp`
1362. `pcbnew\tools\drawing_stackup_table_tool.cpp`
1363. `pcbnew\tools\drawing_tool.cpp`
1364. `pcbnew\tools\drc_tool.cpp`
1365. `pcbnew\tools\edit_tool.cpp`
1366. `pcbnew\tools\edit_tool_move_fct.cpp`
1367. `pcbnew\tools\footprint_chooser_selection_tool.cpp`
1368. `pcbnew\tools\footprint_editor_control.cpp`
1369. `pcbnew\tools\generator_tool.cpp`
1370. `pcbnew\tools\generator_tool_pns_proxy.cpp`
1371. `pcbnew\tools\global_edit_tool.cpp`
1372. `pcbnew\tools\group_tool.cpp`
1373. `pcbnew\tools\item_modification_routine.cpp`
1374. `pcbnew\tools\multichannel_tool.cpp`
1375. `pcbnew\tools\pad_tool.cpp`
1376. `pcbnew\tools\pcb_actions.cpp`
1377. `pcbnew\tools\pcb_control.cpp`
1378. `pcbnew\tools\pcb_edit_table_tool.cpp`
1379. `pcbnew\tools\pcb_editor_conditions.cpp`
1380. `pcbnew\tools\pcb_grid_helper.cpp`
1381. `pcbnew\tools\pcb_picker_tool.cpp`
1382. `pcbnew\tools\pcb_point_editor.cpp`
1383. `pcbnew\tools\pcb_selection.cpp`
1384. `pcbnew\tools\pcb_selection_conditions.cpp`
1385. `pcbnew\tools\pcb_selection_tool.cpp`
1386. `pcbnew\tools\pcb_tool_base.cpp`
1387. `pcbnew\tools\pcb_tool_utils.cpp`
1388. `pcbnew\tools\pcb_viewer_tools.cpp`
1389. `pcbnew\tools\position_relative_tool.cpp`
1390. `pcbnew\tools\tool_event_utils.cpp`
1391. `pcbnew\tools\zone_create_helper.cpp`
1392. `pcbnew\tools\zone_filler_tool.cpp`
1393. `pcbnew\tracks_cleaner.cpp`
1394. `pcbnew\undo_redo.cpp`
1395. `pcbnew\widgets\appearance_controls.cpp`
1396. `pcbnew\widgets\appearance_controls_base.cpp`
1397. `pcbnew\widgets\net_inspector_panel.cpp`
1398. `pcbnew\widgets\panel_footprint_chooser.cpp`
1399. `pcbnew\widgets\panel_selection_filter.cpp`
1400. `pcbnew\widgets\panel_selection_filter_base.cpp`
1401. `pcbnew\widgets\pcb_net_inspector_panel.cpp`
1402. `pcbnew\widgets\pcb_properties_panel.cpp`
1403. `pcbnew\widgets\pcb_search_pane.cpp`
1404. `pcbnew\widgets\search_handlers.cpp`
1405. `pcbnew\zone.cpp`
1406. `pcbnew\zone_filler.cpp`
1407. `pcbnew\zone_manager\board_edges_bounding_item.cpp`
1408. `pcbnew\zone_manager\dialog_zone_manager.cpp`
1409. `pcbnew\zone_manager\dialog_zone_manager_base.cpp`
1410. `pcbnew\zone_manager\model_zones_overview_table.cpp`
1411. `pcbnew\zone_manager\pane_zone_viewer.cpp`
1412. `pcbnew\zone_manager\panel_zone_gal.cpp`
1413. `pcbnew\zone_manager\panel_zone_properties.cpp`
1414. `pcbnew\zone_manager\panel_zone_properties_base.cpp`
1415. `pcbnew\zone_manager\zone_manager_preference.cpp`
1416. `pcbnew\zone_manager\zone_painter.cpp`
1417. `pcbnew\zone_manager\zones_container.cpp`
1418. `pcbnew\zone_settings.cpp`
1419. `plugins\ldr\3d\pluginldr3D.cpp`
1420. `plugins\ldr\pluginldr.cpp`
1421. `scripting\python_manager.cpp`
1422. `scripting\python_scripting.cpp`
1423. `thirdparty\3dxware_sdk\src\navlib_load.cpp`
1424. `thirdparty\3dxware_sdk\src\navlib_stub.c`
1425. `thirdparty\clipper2\Clipper2Lib\src\clipper.engine.cpp`
1426. `thirdparty\clipper2\Clipper2Lib\src\clipper.offset.cpp`
1427. `thirdparty\delaunator\delaunator.cpp`
1428. `thirdparty\dxflib_qcad\dl_dxf.cpp`
1429. `thirdparty\dxflib_qcad\dl_writer_ascii.cpp`
1430. `thirdparty\fmt\src\format.cc`
1431. `thirdparty\json_schema_validator\json-patch.cpp`
1432. `thirdparty\json_schema_validator\json-uri.cpp`
1433. `thirdparty\json_schema_validator\json-validator.cpp`
1434. `thirdparty\libcontext\libcontext.cpp`
1435. `thirdparty\markdown2html\html_formatter\houdini_href_e.c`
1436. `thirdparty\markdown2html\html_formatter\houdini_html_e.c`
1437. `thirdparty\markdown2html\html_formatter\html.c`
1438. `thirdparty\markdown2html\markdown2html.cpp`
1439. `thirdparty\markdown2html\md_parser\autolink.c`
1440. `thirdparty\markdown2html\md_parser\buffer.c`
1441. `thirdparty\markdown2html\md_parser\markdown.c`
1442. `thirdparty\markdown2html\md_parser\stack.c`
1443. `thirdparty\nanodbc\nanodbc\nanodbc.cpp`
1444. `thirdparty\nanosvg\nanosvg.cpp`
1445. `thirdparty\other_math\math_for_graphics.cpp`
1446. `thirdparty\potrace\src\curve.cpp`
1447. `thirdparty\potrace\src\decompose.cpp`
1448. `thirdparty\potrace\src\potracelib.cpp`
1449. `thirdparty\potrace\src\trace.cpp`
1450. `thirdparty\tinyspline_lib\parson.c`
1451. `thirdparty\tinyspline_lib\tinyspline.c`
1452. `thirdparty\tinyspline_lib\tinysplinecxx.cxx`
1453. `utils\idftools\idf_common.cpp`
1454. `utils\idftools\idf_helpers.cpp`
1455. `utils\idftools\idf_outlines.cpp`
1456. `utils\idftools\idf_parser.cpp`
1457. `utils\idftools\vrml_layer.cpp`

## 📁 最小头文件闭包 (.h/.hpp)

以下头文件是上述源文件的完整依赖闭包：

### 3d-viewer/

- `3d-viewer\3d_fastmath.h`
- `3d-viewer\3d_math.h`
- `3d-viewer\3d_viewer_id.h`

### 3d-viewer\3d_cache/

- `3d-viewer\3d_cache\3d_cache.h`
- `3d-viewer\3d_cache\3d_info.h`
- `3d-viewer\3d_cache\3d_plugin_manager.h`

### 3d-viewer\3d_cache\sg/

- `3d-viewer\3d_cache\sg\scenegraph.h`
- `3d-viewer\3d_cache\sg\sg_appearance.h`
- `3d-viewer\3d_cache\sg\sg_colors.h`
- `3d-viewer\3d_cache\sg\sg_coordindex.h`
- `3d-viewer\3d_cache\sg\sg_coords.h`
- `3d-viewer\3d_cache\sg\sg_faceset.h`
- `3d-viewer\3d_cache\sg\sg_helpers.h`
- `3d-viewer\3d_cache\sg\sg_index.h`
- `3d-viewer\3d_cache\sg\sg_node.h`
- `3d-viewer\3d_cache\sg\sg_normals.h`
- `3d-viewer\3d_cache\sg\sg_shape.h`

### 3d-viewer\3d_canvas/

- `3d-viewer\3d_canvas\board_adapter.h`
- `3d-viewer\3d_canvas\eda_3d_canvas.h`

### 3d-viewer\3d_model_viewer/

- `3d-viewer\3d_model_viewer\eda_3d_model_viewer.h`

### 3d-viewer\3d_navlib/

- `3d-viewer\3d_navlib\nl_3d_viewer_plugin.h`
- `3d-viewer\3d_navlib\nl_3d_viewer_plugin_impl.h`
- `3d-viewer\3d_navlib\nl_footprint_properties_plugin.h`
- `3d-viewer\3d_navlib\nl_footprint_properties_plugin_impl.h`

### 3d-viewer\3d_rendering/

- `3d-viewer\3d_rendering\buffers_debug.h`
- `3d-viewer\3d_rendering\color_rgba.h`
- `3d-viewer\3d_rendering\image.h`
- `3d-viewer\3d_rendering\post_shader.h`
- `3d-viewer\3d_rendering\post_shader_ssao.h`
- `3d-viewer\3d_rendering\render_3d_base.h`
- `3d-viewer\3d_rendering\track_ball.h`
- `3d-viewer\3d_rendering\trackball.h`

### 3d-viewer\3d_rendering\opengl/

- `3d-viewer\3d_rendering\opengl\3d_model.h`
- `3d-viewer\3d_rendering\opengl\layer_triangles.h`
- `3d-viewer\3d_rendering\opengl\opengl_utils.h`
- `3d-viewer\3d_rendering\opengl\render_3d_opengl.h`

### 3d-viewer\3d_rendering\raytracing/

- `3d-viewer\3d_rendering\raytracing\PerlinNoise.h`
- `3d-viewer\3d_rendering\raytracing\frustum.h`
- `3d-viewer\3d_rendering\raytracing\hitinfo.h`
- `3d-viewer\3d_rendering\raytracing\light.h`
- `3d-viewer\3d_rendering\raytracing\material.h`
- `3d-viewer\3d_rendering\raytracing\mortoncodes.h`
- `3d-viewer\3d_rendering\raytracing\ray.h`
- `3d-viewer\3d_rendering\raytracing\raypacket.h`
- `3d-viewer\3d_rendering\raytracing\render_3d_raytrace_base.h`
- `3d-viewer\3d_rendering\raytracing\render_3d_raytrace_gl.h`
- `3d-viewer\3d_rendering\raytracing\render_3d_raytrace_ram.h`

### 3d-viewer\3d_rendering\raytracing\accelerators/

- `3d-viewer\3d_rendering\raytracing\accelerators\accelerator_3d.h`
- `3d-viewer\3d_rendering\raytracing\accelerators\bvh_pbrt.h`
- `3d-viewer\3d_rendering\raytracing\accelerators\container_2d.h`
- `3d-viewer\3d_rendering\raytracing\accelerators\container_3d.h`

### 3d-viewer\3d_rendering\raytracing\shapes2D/

- `3d-viewer\3d_rendering\raytracing\shapes2D\4pt_polygon_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\bbox_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\filled_circle_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\layer_item_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\object_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\polygon_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\ring_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\round_segment_2d.h`
- `3d-viewer\3d_rendering\raytracing\shapes2D\triangle_2d.h`

### 3d-viewer\3d_rendering\raytracing\shapes3D/

- `3d-viewer\3d_rendering\raytracing\shapes3D\bbox_3d.h`
- `3d-viewer\3d_rendering\raytracing\shapes3D\cylinder_3d.h`
- `3d-viewer\3d_rendering\raytracing\shapes3D\layer_item_3d.h`
- `3d-viewer\3d_rendering\raytracing\shapes3D\object_3d.h`
- `3d-viewer\3d_rendering\raytracing\shapes3D\plane_3d.h`
- `3d-viewer\3d_rendering\raytracing\shapes3D\round_segment_3d.h`
- `3d-viewer\3d_rendering\raytracing\shapes3D\triangle_3d.h`

### 3d-viewer\3d_viewer/

- `3d-viewer\3d_viewer\eda_3d_viewer_frame.h`
- `3d-viewer\3d_viewer\eda_3d_viewer_settings.h`

### 3d-viewer\3d_viewer\tools/

- `3d-viewer\3d_viewer\tools\eda_3d_actions.h`
- `3d-viewer\3d_viewer\tools\eda_3d_conditions.h`
- `3d-viewer\3d_viewer\tools\eda_3d_controller.h`

### 3d-viewer\common_ogl/

- `3d-viewer\common_ogl\ogl_attr_list.h`
- `3d-viewer\common_ogl\ogl_utils.h`
- `3d-viewer\common_ogl\openGL_includes.h`

### 3d-viewer\dialogs/

- `3d-viewer\dialogs\appearance_controls_3D.h`
- `3d-viewer\dialogs\appearance_controls_3D_base.h`
- `3d-viewer\dialogs\dialog_select_3d_model.h`
- `3d-viewer\dialogs\dialog_select_3d_model_base.h`
- `3d-viewer\dialogs\panel_3D_display_options.h`
- `3d-viewer\dialogs\panel_3D_display_options_base.h`
- `3d-viewer\dialogs\panel_3D_opengl_options.h`
- `3d-viewer\dialogs\panel_3D_opengl_options_base.h`
- `3d-viewer\dialogs\panel_3D_raytracing_options.h`
- `3d-viewer\dialogs\panel_3D_raytracing_options_base.h`
- `3d-viewer\dialogs\panel_preview_3d_model.h`
- `3d-viewer\dialogs\panel_preview_3d_model_base.h`

### bitmap2component/

- `bitmap2component\bitmap2cmp_control.h`
- `bitmap2component\bitmap2cmp_frame.h`
- `bitmap2component\bitmap2cmp_panel.h`
- `bitmap2component\bitmap2cmp_panel_base.h`
- `bitmap2component\bitmap2cmp_settings.h`
- `bitmap2component\bitmap2component.h`

### build/

- `build\config.h`
- `build\kicad_build_version.h`

### build\api\cpp\api\board/

- `build\api\cpp\api\board\board.pb.h`
- `build\api\cpp\api\board\board_commands.pb.h`
- `build\api\cpp\api\board\board_types.pb.h`

### build\api\cpp\api\common/

- `build\api\cpp\api\common\envelope.pb.h`

### build\api\cpp\api\common\commands/

- `build\api\cpp\api\common\commands\base_commands.pb.h`
- `build\api\cpp\api\common\commands\editor_commands.pb.h`
- `build\api\cpp\api\common\commands\project_commands.pb.h`

### build\api\cpp\api\common\types/

- `build\api\cpp\api\common\types\base_types.pb.h`
- `build\api\cpp\api\common\types\enums.pb.h`
- `build\api\cpp\api\common\types\project_settings.pb.h`

### build\api\cpp\api\schematic/

- `build\api\cpp\api\schematic\schematic_types.pb.h`

### build\common/

- `build\common\drc_rules_lexer.h`
- `build\common\embedded_files_lexer.h`
- `build\common\lib_table_lexer.h`
- `build\common\netlist_lexer.h`
- `build\common\pcb_lexer.h`
- `build\common\pcb_plot_params_lexer.h`
- `build\common\stroke_params_lexer.h`
- `build\common\template_fieldnames_lexer.h`

### build\common\drawing_sheet/

- `build\common\drawing_sheet\drawing_sheet_lexer.h`

### build\common\libeval_compiler/

- `build\common\libeval_compiler\grammar.h`

### build\eeschema/

- `build\eeschema\schematic_lexer.h`

### build\eeschema\dialogs/

- `build\eeschema\dialogs\dialog_bom_cfg_lexer.h`

### build\include\gal\shaders/

- `build\include\gal\shaders\glsl_kicad_frag.h`
- `build\include\gal\shaders\glsl_kicad_vert.h`
- `build\include\gal\shaders\glsl_smaa_base.h`
- `build\include\gal\shaders\glsl_smaa_pass_1_frag_color.h`
- `build\include\gal\shaders\glsl_smaa_pass_1_frag_luma.h`
- `build\include\gal\shaders\glsl_smaa_pass_1_vert.h`
- `build\include\gal\shaders\glsl_smaa_pass_2_frag.h`
- `build\include\gal\shaders\glsl_smaa_pass_2_vert.h`
- `build\include\gal\shaders\glsl_smaa_pass_3_frag.h`
- `build\include\gal\shaders\glsl_smaa_pass_3_vert.h`

### build\pcbnew\specctra_import_export/

- `build\pcbnew\specctra_import_export\specctra_lexer.h`

### build\vcpkg_installed\x64-windows\debug\lib\mswud\wx/

- `build\vcpkg_installed\x64-windows\debug\lib\mswud\wx\setup.h`

### build\vcpkg_installed\x64-windows\include/

- `build\vcpkg_installed\x64-windows\include\ft2build.h`
- `build\vcpkg_installed\x64-windows\include\git2.h`
- `build\vcpkg_installed\x64-windows\include\zconf.h`
- `build\vcpkg_installed\x64-windows\include\zlib.h`
- `build\vcpkg_installed\x64-windows\include\zstd.h`
- `build\vcpkg_installed\x64-windows\include\zstd_errors.h`

### build\vcpkg_installed\x64-windows\include\GL/

- `build\vcpkg_installed\x64-windows\include\GL\gl.h`
- `build\vcpkg_installed\x64-windows\include\GL\glew.h`
- `build\vcpkg_installed\x64-windows\include\GL\glu.h`
- `build\vcpkg_installed\x64-windows\include\GL\wglew.h`

### build\vcpkg_installed\x64-windows\include\boost/

- `build\vcpkg_installed\x64-windows\include\boost\any.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\call_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\checked_delete.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\compressed_pair.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept_check.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\cstdint.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\function.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\function_equal.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\get_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\is_placeholder.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\limits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mem_fn.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index_container.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index_container_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\next_prior.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\none.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\none_t.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\operators.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\pointee.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\scoped_array.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\static_assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\throw_exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_index.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\version.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\visit_each.hpp`

### build\vcpkg_installed\x64-windows\include\boost\algorithm/

- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string.hpp`

### build\vcpkg_installed\x64-windows\include\boost\algorithm\string/

- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\case_conv.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\classification.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\compare.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\concept.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\constants.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\erase.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\find.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\find_format.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\find_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\finder.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\formatter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\iter_find.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\join.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\predicate.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\predicate_facade.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\replace.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\sequence_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\split.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\std_containers_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\trim.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\yes_no_type.hpp`

### build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail/

- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\case_conv.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\classification.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\find_format.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\find_format_all.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\find_format_store.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\find_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\finder.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\formatter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\predicate.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\replace_storage.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\sequence.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\trim.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\util.hpp`

### build\vcpkg_installed\x64-windows\include\boost\algorithm\string\std/

- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\std\list_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\std\string_traits.hpp`

### build\vcpkg_installed\x64-windows\include\boost\any/

- `build\vcpkg_installed\x64-windows\include\boost\any\bad_any_cast.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\any\fwd.hpp`

### build\vcpkg_installed\x64-windows\include\boost\any\detail/

- `build\vcpkg_installed\x64-windows\include\boost\any\detail\placeholder.hpp`

### build\vcpkg_installed\x64-windows\include\boost\assert/

- `build\vcpkg_installed\x64-windows\include\boost\assert\source_location.hpp`

### build\vcpkg_installed\x64-windows\include\boost\bind/

- `build\vcpkg_installed\x64-windows\include\boost\bind\arg.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\bind.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\mem_fn.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\placeholders.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\std_placeholders.hpp`

### build\vcpkg_installed\x64-windows\include\boost\bind\detail/

- `build\vcpkg_installed\x64-windows\include\boost\bind\detail\bind_cc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\detail\bind_mf2_cc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\detail\bind_mf_cc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\detail\integer_sequence.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\detail\result_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\bind\detail\tuple_for_each.hpp`

### build\vcpkg_installed\x64-windows\include\boost\concept/

- `build\vcpkg_installed\x64-windows\include\boost\concept\assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\usage.hpp`

### build\vcpkg_installed\x64-windows\include\boost\concept\detail/

- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\backward_compatibility.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\concept_def.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\concept_undef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\has_constraints.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\msvc.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config/

- `build\vcpkg_installed\x64-windows\include\boost\config\helper_macros.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\pragma_message.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\user.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\compiler/

- `build\vcpkg_installed\x64-windows\include\boost\config\compiler\visualc.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\detail/

- `build\vcpkg_installed\x64-windows\include\boost\config\detail\cxx_composite.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\select_compiler_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\select_platform_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\select_stdlib_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\suffix.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\no_tr1/

- `build\vcpkg_installed\x64-windows\include\boost\config\no_tr1\cmath.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\no_tr1\memory.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\platform/

- `build\vcpkg_installed\x64-windows\include\boost\config\platform\win32.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\stdlib/

- `build\vcpkg_installed\x64-windows\include\boost\config\stdlib\dinkumware.hpp`

### build\vcpkg_installed\x64-windows\include\boost\container_hash/

- `build\vcpkg_installed\x64-windows\include\boost\container_hash\hash_fwd.hpp`

### build\vcpkg_installed\x64-windows\include\boost\core/

- `build\vcpkg_installed\x64-windows\include\boost\core\addressof.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\bit.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\checked_delete.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\demangle.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\empty_value.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\enable_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\exchange.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\invoke_swap.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\launder.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\no_exceptions_support.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\noncopyable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\nvp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\ref.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\serialization.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\type_name.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\typeinfo.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\use_default.hpp`

### build\vcpkg_installed\x64-windows\include\boost\core\detail/

- `build\vcpkg_installed\x64-windows\include\boost\core\detail\is_same.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\detail\string_view.hpp`

### build\vcpkg_installed\x64-windows\include\boost\detail/

- `build\vcpkg_installed\x64-windows\include\boost\detail\call_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\detail\compressed_pair.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\detail\is_incrementable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\detail\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\exception/

- `build\vcpkg_installed\x64-windows\include\boost\exception\exception.hpp`

### build\vcpkg_installed\x64-windows\include\boost\function/

- `build\vcpkg_installed\x64-windows\include\boost\function\function_base.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\function\function_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\function\function_template.hpp`

### build\vcpkg_installed\x64-windows\include\boost\integer/

- `build\vcpkg_installed\x64-windows\include\boost\integer\integer_log2.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer\integer_mask.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer\static_log2.hpp`

### build\vcpkg_installed\x64-windows\include\boost\iterator/

- `build\vcpkg_installed\x64-windows\include\boost\iterator\advance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\distance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\enable_if_convertible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\interoperable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\is_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_adaptor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_categories.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_concepts.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_facade.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\reverse_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\transform_iterator.hpp`

### build\vcpkg_installed\x64-windows\include\boost\iterator\detail/

- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\config_def.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\config_undef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\eval_if_default.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\facade_iterator_category.hpp`

### build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits/

- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\conjunction.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\disjunction.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\negation.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\type_identity.hpp`

### build\vcpkg_installed\x64-windows\include\boost\locale/

- `build\vcpkg_installed\x64-windows\include\boost\locale\boundary.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\collator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\conversion.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\date_time.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\date_time_facet.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\encoding.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\encoding_errors.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\encoding_utf.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\format.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\formatting.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\gnu_gettext.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\hold_ptr.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\info.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\localization_backend.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\message.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\time_zone.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\utf.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\util.hpp`

### build\vcpkg_installed\x64-windows\include\boost\locale\boundary/

- `build\vcpkg_installed\x64-windows\include\boost\locale\boundary\boundary_point.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\boundary\facets.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\boundary\index.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\boundary\segment.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\boundary\types.hpp`

### build\vcpkg_installed\x64-windows\include\boost\locale\detail/

- `build\vcpkg_installed\x64-windows\include\boost\locale\detail\allocator_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\detail\any_string.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\detail\encoding.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\detail\facet_id.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\detail\is_supported_char.hpp`

### build\vcpkg_installed\x64-windows\include\boost\locale\util/

- `build\vcpkg_installed\x64-windows\include\boost\locale\util\locale_data.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\locale\util\string.hpp`

### build\vcpkg_installed\x64-windows\include\boost\move/

- `build\vcpkg_installed\x64-windows\include\boost\move\core.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\move\utility_core.hpp`

### build\vcpkg_installed\x64-windows\include\boost\move\detail/

- `build\vcpkg_installed\x64-windows\include\boost\move\detail\addressof.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\move\detail\config_begin.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\move\detail\config_end.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\move\detail\meta_utils.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\move\detail\meta_utils_core.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\move\detail\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mp11/

- `build\vcpkg_installed\x64-windows\include\boost\mp11\integral.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\utility.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\version.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mp11\detail/

- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_defer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_fold.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_front.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_list.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_rename.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_value.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\O1_size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\O1_size_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\advance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\advance_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\always.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\and.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\apply.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\apply_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\apply_wrap.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\arg.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\arg_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\at.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\at_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\back_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\back_inserter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\base.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\begin_end.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\begin_end_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\bind.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\bind_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\bool.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\bool_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\clear.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\clear_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\contains.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\contains_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\deref.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\distance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\distance_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\empty_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\erase_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\erase_key_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\eval_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\find.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\find_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\fold.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\front_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\front_inserter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\has_key.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\has_key_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\has_xxx.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\insert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\insert_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\insert_range_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\inserter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\int.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\int_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\integral_c.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\integral_c_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\integral_c_tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\is_sequence.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\iter_fold.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\iter_fold_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\iterator_category.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\iterator_range.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\iterator_tags.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\key_type_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\lambda.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\lambda_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\less.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\logical.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\long.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\long_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\min_max.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\minus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\negate.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\next.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\next_prior.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\not.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\numeric_cast.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\or.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\pair.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\pair_view.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\placeholders.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\plus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\pop_back_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\pop_front_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\prior.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\protect.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\push_back.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\push_back_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\push_front.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\push_front_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\quote.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\reverse_fold.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\reverse_iter_fold.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\same_as.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\sequence_tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\sequence_tag_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\size_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\transform.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\value_type_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\void.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\void_fwd.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\O1_size_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\adl_barrier.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\advance_backward.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\advance_forward.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\arg_typedef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\arithmetic_op.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\arity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\arity_spec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\at_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\begin_end_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\clear_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\common_name_wknd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\comparison_op.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\contains_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\empty_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\find_if_pred.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\full_lambda.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\has_apply.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\has_begin.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\has_key_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\has_size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\has_tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\has_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\include_preprocessed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\insert_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\inserter_algorithm.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\integral_wrapper.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\is_msvc_eti_arg.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\iter_apply.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\iter_fold_if_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\iter_fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\lambda_arity_param.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\lambda_spec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\lambda_support.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\largest_int.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\msvc_eti_base.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\msvc_never_true.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\msvc_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na_assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na_spec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\nested_type_wknd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\nttp_decl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\numeric_cast_utils.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\numeric_op.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\overload_names.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\ptr_to_ref.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\push_back_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\push_front_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\reverse_fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\reverse_iter_fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\size_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\static_cast.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\template_arity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\template_arity_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\traits_lambda_spec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\type_wrapper.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\value_wknd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\yes_no.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\adl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\arrays.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\bcc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\bind.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\compiler.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\ctps.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\dtp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\eti.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\forwarding.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\gcc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\gpu.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\has_apply.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\has_xxx.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\integral.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\intel.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\lambda.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\msvc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\msvc_typename.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\nttp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\operators.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\overload_resolution.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\pp_counter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\preprocessor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\static_constant.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\ttp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\typeof.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\use_preprocessed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\advance_backward.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\advance_forward.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\and.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\apply.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\apply_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\apply_wrap.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\arg.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\bind.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\bind_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\full_lambda.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\iter_fold_if_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\iter_fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\less.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\minus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\or.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\placeholders.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\plus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\quote.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\reverse_fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\reverse_iter_fold_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\template_arity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\vector.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\def_params_tail.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\default_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\params.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\limits/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\limits\arity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\limits\vector.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\set/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\set0.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\at_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\begin_end_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\clear_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\empty_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\erase_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\erase_key_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\has_key_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\insert_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\insert_range_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\item.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\key_type_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\set0.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\size_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\set\aux_\value_type_impl.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\vector/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\vector0.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\vector10.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\vector20.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\O1_size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\at.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\back.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\begin_end.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\clear.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\front.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\include_preprocessed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\item.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\pop_back.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\pop_front.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\push_back.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\push_front.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\vector0.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\preprocessed\plain/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\preprocessed\plain\vector10.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\vector\aux_\preprocessed\plain\vector20.hpp`

### build\vcpkg_installed\x64-windows\include\boost\multi_index/

- `build\vcpkg_installed\x64-windows\include\boost\multi_index\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\identity_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\indexed_by.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\member.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\ordered_index.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\ordered_index_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\sequenced_index.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\sequenced_index_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\tag.hpp`

### build\vcpkg_installed\x64-windows\include\boost\multi_index\detail/

- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\access_specifier.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\adl_swap.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\allocator_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\archive_constructed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\auto_space.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\bad_archive_exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\base_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\bidir_node_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\converter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\copy_map.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\define_if_constexpr_macro.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\do_not_copy_elements_tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\duplicates_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\has_tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\header_holder.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\ignore_wstrict_aliasing.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\index_access_sequence.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\index_base.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\index_loader.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\index_matcher.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\index_node_base.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\index_saver.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\invalidate_iterators.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\is_index_list.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\is_transparent.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\modify_key_adaptor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\no_duplicate_tags.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\node_handle.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\node_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\ord_index_args.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\ord_index_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\ord_index_impl_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\ord_index_node.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\ord_index_ops.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\promotes_arg.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\raw_ptr.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\restore_wstrict_aliasing.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\safe_mode.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\scope_guard.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\seq_index_node.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\seq_index_ops.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\serialization_version.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\uintptr_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\unbounded.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\undef_if_constexpr_macro.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\value_compare.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\multi_index\detail\vartempl_support.hpp`

### build\vcpkg_installed\x64-windows\include\boost\optional/

- `build\vcpkg_installed\x64-windows\include\boost\optional\bad_optional_access.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\optional.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\optional_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\optional_io.hpp`

### build\vcpkg_installed\x64-windows\include\boost\optional\detail/

- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_aligned_storage.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_factory_support.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_hash.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_reference_spec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_relops.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_swap.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_trivially_copyable_base.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\optional\detail\optional_utility.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\cat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\comma_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\inc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repeat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\stringize.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\add.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\dec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\inc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\mod.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\sub.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\detail\div_base.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\array/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\array\data.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\array\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\array\size.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\comparison/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\comparison\less_equal.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\config/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\config\config.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\control/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\deduce_d.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\expr_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\expr_iif.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\iif.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\while.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\detail\msvc/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\detail\msvc\while.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\debug/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\debug\error.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail\auto_rec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail\check.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail\is_binary.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\check_empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\expand.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\intercept.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\is_1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\is_empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\is_empty_variadic.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\overload.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\detail\is_empty.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\list/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\adt.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\fold_left.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\fold_right.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\reverse.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\detail\fold_left.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\detail\fold_right.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\and.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\bitand.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\bool.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\compl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\not.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\comma.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\comma_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\is_begin_parens.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\detail\is_begin_parens.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum_binary_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum_trailing_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\for.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\repeat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\repeat_from_to.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\detail\msvc/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\detail\msvc\for.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\cat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\fold_left.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\for_each_i.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\seq.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\transform.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\detail\is_empty.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\eat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\rem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\size.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\detail\is_single_return.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\has_opt.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\size.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\detail\has_opt.hpp`

### build\vcpkg_installed\x64-windows\include\boost\property_tree/

- `build\vcpkg_installed\x64-windows\include\boost\property_tree\exceptions.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\id_translator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\ptree.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\ptree_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\stream_translator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\string_path.hpp`

### build\vcpkg_installed\x64-windows\include\boost\property_tree\detail/

- `build\vcpkg_installed\x64-windows\include\boost\property_tree\detail\exception_implementation.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\detail\ptree_implementation.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\property_tree\detail\ptree_utils.hpp`

### build\vcpkg_installed\x64-windows\include\boost\ptr_container/

- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\clone_allocator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\indirect_fun.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\nullable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_map.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_map_adapter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_sequence_adapter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_set.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_set_adapter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_vector.hpp`

### build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail/

- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\associative_ptr_container.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\default_deleter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\is_convertible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\map_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\meta_functions.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\move.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\ptr_container_disable_deprecated.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\reversible_ptr_container.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\scoped_deleter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\static_move_ptr.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\throw_exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\void_ptr_iterator.hpp`

### build\vcpkg_installed\x64-windows\include\boost\random/

- `build\vcpkg_installed\x64-windows\include\boost\random\mersenne_twister.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\traits.hpp`

### build\vcpkg_installed\x64-windows\include\boost\random\detail/

- `build\vcpkg_installed\x64-windows\include\boost\random\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\const_mod.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\disable_warnings.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\enable_warnings.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\generator_bits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\generator_seed_seq.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\integer_log2.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\large_arithmetic.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\polynomial.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\ptr_helper.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\seed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\seed_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\signed_unsigned_tools.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range/

- `build\vcpkg_installed\x64-windows\include\boost\range\as_literal.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\begin.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\concepts.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\const_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\difference_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\distance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\end.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\functions.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\has_range_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator_range.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator_range_core.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator_range_io.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\mutable_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\range_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\rbegin.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\rend.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\reverse_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\size_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\value_type.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range\adaptor/

- `build\vcpkg_installed\x64-windows\include\boost\range\adaptor\reversed.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range\algorithm/

- `build\vcpkg_installed\x64-windows\include\boost\range\algorithm\equal.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\algorithm\nth_element.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\algorithm\partition.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range\detail/

- `build\vcpkg_installed\x64-windows\include\boost\range\detail\common.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\extract_optional_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\has_member_size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\implementation_help.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\misc_concept.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\msvc_has_iterator_workaround.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\range_return.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\safe_bool.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\sfinae.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\str_types.hpp`

### build\vcpkg_installed\x64-windows\include\boost\smart_ptr/

- `build\vcpkg_installed\x64-windows\include\boost\smart_ptr\scoped_array.hpp`

### build\vcpkg_installed\x64-windows\include\boost\smart_ptr\detail/

- `build\vcpkg_installed\x64-windows\include\boost\smart_ptr\detail\deprecated_macros.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\smart_ptr\detail\sp_noexcept.hpp`

### build\vcpkg_installed\x64-windows\include\boost\tuple/

- `build\vcpkg_installed\x64-windows\include\boost\tuple\tuple.hpp`

### build\vcpkg_installed\x64-windows\include\boost\tuple\detail/

- `build\vcpkg_installed\x64-windows\include\boost\tuple\detail\tuple_basic.hpp`

### build\vcpkg_installed\x64-windows\include\boost\type_index/

- `build\vcpkg_installed\x64-windows\include\boost\type_index\stl_type_index.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_index\type_index_facade.hpp`

### build\vcpkg_installed\x64-windows\include\boost\type_traits/

- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_const.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_cv.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_rvalue_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_volatile.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\aligned_storage.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\alignment_of.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\conditional.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\conjunction.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\conversion_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\cv_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\decay.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\declval.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\disjunction.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\enable_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\function_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_minus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_minus_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_nothrow_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_nothrow_constructor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_plus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_plus_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_trivial_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_trivial_constructor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_trivial_move_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\integral_constant.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\intrinsics.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_abstract.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_arithmetic.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_array.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_assignable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_base_and_derived.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_base_of.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_class.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_complete.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_const.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_constructible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_convertible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_copy_constructible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_default_constructible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_destructible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_final.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_floating_point.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_function.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_integral.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_lvalue_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_member_function_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_member_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_nothrow_move_assignable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_nothrow_move_constructible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_pod.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_rvalue_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_same.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_scalar.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_signed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_unsigned.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_void.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_volatile.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\make_unsigned.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\make_void.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_bounds.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_const.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_cv.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_extent.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_volatile.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\type_identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\type_with_alignment.hpp`

### build\vcpkg_installed\x64-windows\include\boost\type_traits\detail/

- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\bool_trait_undef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\has_binary_operator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\is_function_cxx_11.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\is_member_function_pointer_cxx_11.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\yes_no_type.hpp`

### build\vcpkg_installed\x64-windows\include\boost\utility/

- `build\vcpkg_installed\x64-windows\include\boost\utility\base_from_member.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\binary.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\compare_pointees.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\enable_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\identity_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\result_of.hpp`

### build\vcpkg_installed\x64-windows\include\boost\utility\detail/

- `build\vcpkg_installed\x64-windows\include\boost\utility\detail\result_of_variadic.hpp`

### build\vcpkg_installed\x64-windows\include\boost\uuid/

- `build\vcpkg_installed\x64-windows\include\boost\uuid\basic_random_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\entropy_error.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\name_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\name_generator_md5.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\name_generator_sha1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\namespaces.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\nil_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\random_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\string_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator_v1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator_v6.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator_v7.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid_clock.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid_generators.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid_io.hpp`

### build\vcpkg_installed\x64-windows\include\boost\uuid\detail/

- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\basic_name_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\chacha20.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\endian.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\hash_mix.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\md5.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\numeric_cast.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\random_device.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\random_provider.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\sha1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\static_assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\to_chars.hpp`

### build\vcpkg_installed\x64-windows\include\cairo/

- `build\vcpkg_installed\x64-windows\include\cairo\cairo-deprecated.h`
- `build\vcpkg_installed\x64-windows\include\cairo\cairo-features.h`
- `build\vcpkg_installed\x64-windows\include\cairo\cairo-version.h`
- `build\vcpkg_installed\x64-windows\include\cairo\cairo-win32.h`
- `build\vcpkg_installed\x64-windows\include\cairo\cairo.h`

### build\vcpkg_installed\x64-windows\include\curl/

- `build\vcpkg_installed\x64-windows\include\curl\curl.h`
- `build\vcpkg_installed\x64-windows\include\curl\curlver.h`
- `build\vcpkg_installed\x64-windows\include\curl\easy.h`
- `build\vcpkg_installed\x64-windows\include\curl\header.h`
- `build\vcpkg_installed\x64-windows\include\curl\mprintf.h`
- `build\vcpkg_installed\x64-windows\include\curl\multi.h`
- `build\vcpkg_installed\x64-windows\include\curl\options.h`
- `build\vcpkg_installed\x64-windows\include\curl\system.h`
- `build\vcpkg_installed\x64-windows\include\curl\urlapi.h`
- `build\vcpkg_installed\x64-windows\include\curl\websockets.h`

### build\vcpkg_installed\x64-windows\include\fontconfig/

- `build\vcpkg_installed\x64-windows\include\fontconfig\fontconfig.h`

### build\vcpkg_installed\x64-windows\include\freetype/

- `build\vcpkg_installed\x64-windows\include\freetype\freetype.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftbbox.h`
- `build\vcpkg_installed\x64-windows\include\freetype\fterrdef.h`
- `build\vcpkg_installed\x64-windows\include\freetype\fterrors.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftglyph.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftimage.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftmoderr.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftoutln.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftparams.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftsnames.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftsystem.h`
- `build\vcpkg_installed\x64-windows\include\freetype\fttypes.h`
- `build\vcpkg_installed\x64-windows\include\freetype\tttables.h`

### build\vcpkg_installed\x64-windows\include\freetype\config/

- `build\vcpkg_installed\x64-windows\include\freetype\config\ftconfig.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\ftheader.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\ftoption.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\ftstdlib.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\integer-types.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\mac-support.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\public-macros.h`

### build\vcpkg_installed\x64-windows\include\git2/

- `build\vcpkg_installed\x64-windows\include\git2\annotated_commit.h`
- `build\vcpkg_installed\x64-windows\include\git2\apply.h`
- `build\vcpkg_installed\x64-windows\include\git2\attr.h`
- `build\vcpkg_installed\x64-windows\include\git2\blame.h`
- `build\vcpkg_installed\x64-windows\include\git2\blob.h`
- `build\vcpkg_installed\x64-windows\include\git2\branch.h`
- `build\vcpkg_installed\x64-windows\include\git2\buffer.h`
- `build\vcpkg_installed\x64-windows\include\git2\cert.h`
- `build\vcpkg_installed\x64-windows\include\git2\checkout.h`
- `build\vcpkg_installed\x64-windows\include\git2\cherrypick.h`
- `build\vcpkg_installed\x64-windows\include\git2\clone.h`
- `build\vcpkg_installed\x64-windows\include\git2\commit.h`
- `build\vcpkg_installed\x64-windows\include\git2\common.h`
- `build\vcpkg_installed\x64-windows\include\git2\config.h`
- `build\vcpkg_installed\x64-windows\include\git2\credential.h`
- `build\vcpkg_installed\x64-windows\include\git2\credential_helpers.h`
- `build\vcpkg_installed\x64-windows\include\git2\deprecated.h`
- `build\vcpkg_installed\x64-windows\include\git2\describe.h`
- `build\vcpkg_installed\x64-windows\include\git2\diff.h`
- `build\vcpkg_installed\x64-windows\include\git2\email.h`
- `build\vcpkg_installed\x64-windows\include\git2\errors.h`
- `build\vcpkg_installed\x64-windows\include\git2\experimental.h`
- `build\vcpkg_installed\x64-windows\include\git2\filter.h`
- `build\vcpkg_installed\x64-windows\include\git2\global.h`
- `build\vcpkg_installed\x64-windows\include\git2\graph.h`
- `build\vcpkg_installed\x64-windows\include\git2\ignore.h`
- `build\vcpkg_installed\x64-windows\include\git2\index.h`
- `build\vcpkg_installed\x64-windows\include\git2\indexer.h`
- `build\vcpkg_installed\x64-windows\include\git2\mailmap.h`
- `build\vcpkg_installed\x64-windows\include\git2\merge.h`
- `build\vcpkg_installed\x64-windows\include\git2\message.h`
- `build\vcpkg_installed\x64-windows\include\git2\net.h`
- `build\vcpkg_installed\x64-windows\include\git2\notes.h`
- `build\vcpkg_installed\x64-windows\include\git2\object.h`
- `build\vcpkg_installed\x64-windows\include\git2\odb.h`
- `build\vcpkg_installed\x64-windows\include\git2\odb_backend.h`
- `build\vcpkg_installed\x64-windows\include\git2\oid.h`
- `build\vcpkg_installed\x64-windows\include\git2\oidarray.h`
- `build\vcpkg_installed\x64-windows\include\git2\pack.h`
- `build\vcpkg_installed\x64-windows\include\git2\patch.h`
- `build\vcpkg_installed\x64-windows\include\git2\pathspec.h`
- `build\vcpkg_installed\x64-windows\include\git2\proxy.h`
- `build\vcpkg_installed\x64-windows\include\git2\rebase.h`
- `build\vcpkg_installed\x64-windows\include\git2\refdb.h`
- `build\vcpkg_installed\x64-windows\include\git2\reflog.h`
- `build\vcpkg_installed\x64-windows\include\git2\refs.h`
- `build\vcpkg_installed\x64-windows\include\git2\refspec.h`
- `build\vcpkg_installed\x64-windows\include\git2\remote.h`
- `build\vcpkg_installed\x64-windows\include\git2\repository.h`
- `build\vcpkg_installed\x64-windows\include\git2\reset.h`
- `build\vcpkg_installed\x64-windows\include\git2\revert.h`
- `build\vcpkg_installed\x64-windows\include\git2\revparse.h`
- `build\vcpkg_installed\x64-windows\include\git2\revwalk.h`
- `build\vcpkg_installed\x64-windows\include\git2\signature.h`
- `build\vcpkg_installed\x64-windows\include\git2\stash.h`
- `build\vcpkg_installed\x64-windows\include\git2\status.h`
- `build\vcpkg_installed\x64-windows\include\git2\strarray.h`
- `build\vcpkg_installed\x64-windows\include\git2\submodule.h`
- `build\vcpkg_installed\x64-windows\include\git2\tag.h`
- `build\vcpkg_installed\x64-windows\include\git2\trace.h`
- `build\vcpkg_installed\x64-windows\include\git2\transaction.h`
- `build\vcpkg_installed\x64-windows\include\git2\transport.h`
- `build\vcpkg_installed\x64-windows\include\git2\tree.h`
- `build\vcpkg_installed\x64-windows\include\git2\types.h`
- `build\vcpkg_installed\x64-windows\include\git2\version.h`
- `build\vcpkg_installed\x64-windows\include\git2\worktree.h`

### build\vcpkg_installed\x64-windows\include\git2\sys/

- `build\vcpkg_installed\x64-windows\include\git2\sys\credential.h`

### build\vcpkg_installed\x64-windows\include\glm/

- `build\vcpkg_installed\x64-windows\include\glm\common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\exponential.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\geometric.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\glm.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\integer.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\matrix.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\packing.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\trigonometric.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vec2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vec3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vec4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vector_relational.hpp`

### build\vcpkg_installed\x64-windows\include\glm\detail/

- `build\vcpkg_installed\x64-windows\include\glm\detail\_fixes.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\_noise.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\_vectorize.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\compute_common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\compute_vector_relational.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_common.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_exponential.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_geometric.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_integer.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_matrix.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_packing.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_trigonometric.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_vector_relational.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\qualifier.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\setup.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_float.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_half.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_half.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x4.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x4.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x4.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_quat.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_quat.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec1.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec4.inl`

### build\vcpkg_installed\x64-windows\include\glm\ext/

- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_clip_space.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_clip_space.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_common.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int2x2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int2x3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int2x4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int3x2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int3x3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int3x4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int4x2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int4x3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_int4x4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_projection.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_projection.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_relational.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_relational.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_transform.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_transform.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint2x2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint2x3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint2x4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint3x2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint3x3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint3x4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint4x2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint4x3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_uint4x4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_common.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_double.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_double_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_exponential.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_exponential.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_float.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_float_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_geometric.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_geometric.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_relational.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_relational.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_transform.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_transform.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_trigonometric.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\quaternion_trigonometric.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_common.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_constants.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_constants.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_int_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_integer.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_integer.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_packing.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_packing.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_relational.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_relational.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_uint_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_ulp.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_ulp.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool1_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_common.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double1_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float1_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int1_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_integer.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_integer.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_packing.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_packing.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_relational.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_relational.inl`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint1_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_ulp.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_ulp.inl`

### build\vcpkg_installed\x64-windows\include\glm\gtc/

- `build\vcpkg_installed\x64-windows\include\glm\gtc\bitfield.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\bitfield.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\color_space.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\color_space.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\constants.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\constants.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\epsilon.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\epsilon.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\integer.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\integer.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_access.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_access.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_integer.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_inverse.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_inverse.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_transform.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\matrix_transform.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\noise.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\noise.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\packing.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\packing.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\quaternion.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\quaternion.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\random.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\random.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\reciprocal.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\reciprocal.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\round.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\round.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\type_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\type_precision.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\type_ptr.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\type_ptr.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\ulp.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\ulp.inl`
- `build\vcpkg_installed\x64-windows\include\glm\gtc\vec1.hpp`

### build\vcpkg_installed\x64-windows\include\glm\simd/

- `build\vcpkg_installed\x64-windows\include\glm\simd\integer.h`
- `build\vcpkg_installed\x64-windows\include\glm\simd\platform.h`

### build\vcpkg_installed\x64-windows\include\google\protobuf/

- `build\vcpkg_installed\x64-windows\include\google\protobuf\any.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\any.pb.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arena.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arena_impl.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arenastring.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arenaz_sampler.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\descriptor.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\empty.pb.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\endian.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\explicitly_constructed.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\extension_set.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\field_mask.pb.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_enum_reflection.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_enum_util.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_message_bases.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_message_reflection.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_message_util.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\has_bits.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\implicit_weak_message.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\inlined_string_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_entry.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_entry_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_field_inl.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_field_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_type_handler.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\message.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\message_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\metadata_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\parse_context.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\port.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\port_def.inc`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\port_undef.inc`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\reflection_ops.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\repeated_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\repeated_ptr_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\unknown_field_set.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\wire_format.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\wire_format_lite.h`

### build\vcpkg_installed\x64-windows\include\google\protobuf\io/

- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\coded_stream.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\zero_copy_stream.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\zero_copy_stream_impl.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\zero_copy_stream_impl_lite.h`

### build\vcpkg_installed\x64-windows\include\google\protobuf\stubs/

- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\callback.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\casts.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\common.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\hash.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\logging.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\macros.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\mutex.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\once.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\platform_macros.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\port.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\status.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\stl_util.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\stringpiece.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\strutil.h`

### build\vcpkg_installed\x64-windows\include\harfbuzz/

- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-blob.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-buffer.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-common.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-deprecated.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-draw.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-face.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-font.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-ft.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-map.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-paint.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-set.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-shape-plan.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-shape.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-style.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-unicode.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-version.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb.h`

### build\vcpkg_installed\x64-windows\include\ngspice/

- `build\vcpkg_installed\x64-windows\include\ngspice\config.h`
- `build\vcpkg_installed\x64-windows\include\ngspice\sharedspice.h`

### build\vcpkg_installed\x64-windows\include\nng/

- `build\vcpkg_installed\x64-windows\include\nng\nng.h`

### build\vcpkg_installed\x64-windows\include\nng\protocol\reqrep0/

- `build\vcpkg_installed\x64-windows\include\nng\protocol\reqrep0\rep.h`

### build\vcpkg_installed\x64-windows\include\opencascade/

- `build\vcpkg_installed\x64-windows\include\opencascade\APIHeaderSection_MakeHeader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Adaptor3d_Curve.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Adaptor3d_Surface.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_CheckResult.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_CheckStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_GlueEnum.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_ListOfCheckResult.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_Operation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_Options.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_PBuilder.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BOPAlgo_PPaveFiller.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAdaptor_Surface.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAlgoAPI_Algo.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAlgoAPI_BooleanOperation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAlgoAPI_BuilderAlgo.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAlgoAPI_Check.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAlgoAPI_Cut.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepAlgoAPI_Fuse.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBndLib.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_Collect.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_Command.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_EdgeError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_FaceError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_GTransform.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_MakeEdge.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_MakeFace.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_MakeShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_MakeWire.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_ModifyShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepBuilderAPI_WireError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepExtrema_DistShapeShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepExtrema_SeqOfSolution.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepExtrema_SolutionElem.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepExtrema_SupportType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepGProp.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_Command.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_EdgeError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_FaceError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_MakeEdge.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_MakeFace.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_MakeShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_MakeWire.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_ShapeModification.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepLib_WireError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_Circle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_DegreeOfFreedom.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_DiscretRoot.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_Edge.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_IncrementalMesh.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_OrientedEdge.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_PairOfIndex.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_Triangle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepMesh_Vertex.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepPrimAPI_MakePrism.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepPrimAPI_MakeSweep.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepSweep_Builder.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepSweep_NumLinearRegularSweep.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepSweep_Prism.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepSweep_Tool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepSweep_Translation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepSweep_Trsf.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepTools.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepTools_History.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRepTools_Modifier.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRep_Builder.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRep_ListOfPointRepresentation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRep_PointRepresentation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRep_TFace.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRep_TVertex.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BRep_Tool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BSplCLib.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BSplCLib_CacheParams.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BSplCLib_EvaluatorFunction.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BSplCLib_KnotDistribution.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BSplCLib_MultDistribution.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BSplSLib_Cache.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BVH_Box.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BVH_Constants.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\BVH_Types.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Bnd_Array1OfBox.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Bnd_B2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Bnd_BoundSortBox.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Bnd_Box.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Bnd_Box2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Bnd_HArray1OfBox.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDF_Application.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDF_MetaDataDriver.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDF_TypeOfActivation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_Application.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_CanCloseStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_Document.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_DocumentPointer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_ListOfDocument.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_ListOfReferences.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_MetaDataLookUpTable.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\CDM_Reference.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\DESTEP_Parameters.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\DE_ShapeFixParameters.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Extrema_ExtAlgo.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Extrema_ExtFlag.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GC_MakeArcOfCircle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GC_MakeCircle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GC_Root.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GProp_GProps.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom2d_Curve.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom2d_Geometry.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GeomAbs_BSplKnotDistribution.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GeomAbs_CurveType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GeomAbs_Shape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GeomAbs_SurfaceType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GeomAdaptor_Surface.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\GeomEvaluator_Surface.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_BoundedCurve.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_Circle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_Conic.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_Curve.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_ElementarySurface.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_Geometry.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_Plane.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_Surface.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Geom_TrimmedCurve.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_AlphaMode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_BndBox3d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_Mat4.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_Mat4d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_TypeOfBackfacingModel.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_Vec.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_Vec2.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_Vec3.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Graphic3d_Vec4.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IFSelect_PrintCount.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IFSelect_PrintFail.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IFSelect_ReturnStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESCAFControl_Reader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESCAFControl_Writer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESControl_Controller.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESControl_Reader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESControl_Writer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESData_BasicEditor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESData_GlobalSection.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESData_IGESModel.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IGESData_SpecificLib.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshData_Model.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshData_ParametersListArrayAdaptor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshData_Shape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshData_Types.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshTools_Context.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshTools_MeshAlgoType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshTools_ModelAlgo.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshTools_ModelBuilder.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\IMeshTools_Parameters.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Image_Texture.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_Array1OfHAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_DataState.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_GeneralLib.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_HArray1OfHAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_InterfaceModel.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_ParamType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_Static.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_StaticSatisfies.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Interface_TypedValue.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Alert.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_AlertExtended.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Algorithm.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_ConsoleColor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_ExecStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Gravity.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_HArrayOfMsg.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Level.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_ListOfAlert.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Messenger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_MetricType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Msg.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Printer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_PrinterOStream.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_ProgressIndicator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_ProgressRange.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_ProgressScope.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Report.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_SequenceOfPrinters.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_Status.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Message_StatusType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\MoniTool_DataMapOfShapeTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\MoniTool_TypedValue.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\MoniTool_ValueInterpret.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\MoniTool_ValueSatisfies.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\MoniTool_ValueType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_AliasedArray.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Allocator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Array1.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Array2.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_BaseAllocator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_BaseList.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_BaseMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_BasePointerVector.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_BaseSequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Buffer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_CellFilter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DataMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DefaultHasher.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DefineAlloc.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DefineHArray1.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DefineHArray2.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DefineHSequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DefineHasher.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_DynamicArray.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_EBTree.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Handle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_IncAllocator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_IndexedDataMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_IndexedIterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_IndexedMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Iterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_List.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_ListNode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_LocalArray.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Map.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_MapAlgo.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Mat3.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Mat4.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_OccAllocator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Sequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Shared.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_StlIterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_TListIterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_TListNode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_UBTree.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_UBTreeFiller.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_UtfIterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_UtfString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Vec2.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Vec3.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Vec4.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\NCollection_Vector.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\OSD_MemInfo.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\OSD_OpenFile.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\PCDM_ReaderStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\PCDM_StoreStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\PLib.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_Array1OfTriangle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_ArrayOfNodes.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_ArrayOfUVNodes.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_HArray1OfTriangle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_ListOfTriangulation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_MeshPurpose.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_Polygon2D.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_Polygon3D.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_PolygonOnTriangulation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_Triangle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Poly_Triangulation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Precision.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Quantity_Color.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Quantity_ColorRGBA.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Quantity_NameOfColor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Quantity_TypeOfColor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_CafWriter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_DracoParameters.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfAccessor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfAccessorCompType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfAccessorLayout.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfArrayType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfBufferView.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfBufferViewTarget.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_GltfFace.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWGltf_WriterTrsfFormat.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWMesh_CafReader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWMesh_CoordinateSystem.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWMesh_CoordinateSystemConverter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWMesh_NameFormat.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWMesh_NodeAttributes.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\RWPly_CafWriter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Resource_FormatType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_DataMapOfLabelExternFile.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_DataMapOfLabelShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_DataMapOfPDExternFile.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_DataMapOfShapePD.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_ExternFile.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_Reader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPCAFControl_Writer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPControl_Reader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPControl_StepModelType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\STEPControl_Writer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\ShapeProcess.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\ShapeUpgrade_UnifySameDomain.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Assert.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Boolean.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_CString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Character.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_ConstructionError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_DefineAlloc.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_DefineException.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_DefineHandle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_DimensionError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_DimensionMismatch.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_DomainError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Dump.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_ErrorHandler.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_ExtCharacter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Failure.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_GUID.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Handle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_HandlerStatus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_HashUtils.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_IStream.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Integer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_JmpBuf.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Macro.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Mutex.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_NoSuchObject.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_NotImplemented.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_NullObject.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_NumericError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_OStream.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_OutOfMemory.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_OutOfRange.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_PCharacter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_PErrorHandler.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_PExtCharacter.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_PrimitiveTypes.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_ProgramError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_RangeError.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Real.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_SStream.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_ShortReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Std.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Stream.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_ThreadId.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Transient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Type.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_TypeDef.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_TypeMismatch.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_UUID.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Version.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StdFail_NotDone.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepAP242_GeometricItemSpecificUsage.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepAP242_ItemIdentifiedRepresentationUsage.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepAP242_ItemIdentifiedRepresentationUsageDefinition.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepBasic_ProductDefinition.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepData_Factors.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepData_Logical.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepData_SelectType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepDimTol_Array1OfDatumSystemOrReference.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepDimTol_Datum.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepDimTol_DatumSystemOrReference.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepDimTol_GeometricTolerance.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepDimTol_GeometricToleranceTarget.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepDimTol_HArray1OfDatumSystemOrReference.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_Array1OfRepresentationItem.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_CharacterizedDefinition.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_HArray1OfRepresentationItem.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_ProductDefinitionShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_PropertyDefinition.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_Representation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_RepresentationItem.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepRepr_ShapeAspect.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_Array1OfPresentationStyleAssignment.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_Array1OfPresentationStyleSelect.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_DraughtingModel.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_HArray1OfPresentationStyleAssignment.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_HArray1OfPresentationStyleSelect.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_PresentationStyleAssignment.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StepVisual_PresentationStyleSelect.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\StlAPI_Writer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Sweep_NumShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\Sweep_NumShapeTool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_Array1OfAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_Array1OfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_Array1OfReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_Array1OfTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_Array2OfBoolean.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_Array2OfReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_DataMapOfIntegerInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_DataMapOfIntegerTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_DataMapOfStringInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HArray1OfAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HArray1OfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HArray1OfReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HArray1OfTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HArray2OfReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HSequenceOfAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HSequenceOfHAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HSequenceOfHExtendedString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_HSequenceOfTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_IndexedDataMapOfStringString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_IndexedMapOfTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_ListOfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_MapIteratorOfPackedMapOfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_MapOfAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_PackedMapOfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_SequenceOfAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_SequenceOfExtendedString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_SequenceOfHAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_SequenceOfHExtendedString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_SequenceOfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColStd_SequenceOfTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColgp_Array1OfPnt.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColgp_Array1OfPnt2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColgp_Array1OfXYZ.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColgp_Array2OfPnt.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColgp_HArray1OfPnt.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TColgp_HArray1OfPnt2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TCollection_AsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TCollection_ExtendedString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TCollection_HAsciiString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TCollection_HExtendedString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_Attribute.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_AttributeDelta.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_AttributeDeltaList.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_AttributeIndexedMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_AttributeMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_AttributeSequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_ChildIterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_Data.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_Delta.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_DeltaList.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_DerivedAttribute.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_HAllocator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_Label.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelIndexedMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelIntegerMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelList.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelMap.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelNode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelNodePtr.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_LabelSequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_TagSource.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_Tool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDF_Transaction.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_DataMapOfStringByte.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_DataMapOfStringHArray1OfInteger.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_DataMapOfStringHArray1OfReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_DataMapOfStringReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_DataMapOfStringString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_GenericEmpty.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_GenericExtString.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_Name.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_NamedData.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_PtrTreeNode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_Real.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_RealEnum.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDataStd_TreeNode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDocStd_Application.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDocStd_Document.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDocStd_FormatVersion.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TDocStd_XLinkTool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TShort_Array1OfShortReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TShort_HArray1OfShortReal.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopAbs.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopAbs_Orientation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopAbs_ShapeEnum.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopAbs_State.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopExp.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopExp_Explorer.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopExp_Stack.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopLoc_Datum3D.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopLoc_ItemLocation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopLoc_Location.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopLoc_SListOfItemLocation.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_Array2OfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_DataMapOfShapeListOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_DataMapOfShapeShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_FormatVersion.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_IndexedDataMapOfShapeListOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_IndexedMapOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_ListOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_MapOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_SequenceOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopTools_ShapeMapHasher.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Builder.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_CompSolid.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Compound.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Edge.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Face.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Iterator.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_ListIteratorOfListOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_ListOfShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Shape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Shell.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Solid.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TCompSolid.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TCompound.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TFace.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TShape.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TShell.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TSolid.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TVertex.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_TWire.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Vertex.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\TopoDS_Wire.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\UnitsMethods_LengthUnit.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\VrmlAPI_CafReader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFApp_Application.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_DatumModifWithValue.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_DatumModifiersSequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_DatumSingleModif.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceMatReqModif.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceModif.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceModifiersSequence.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceObject.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceTypeValue.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_GeomToleranceZoneModif.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDimTolObjects_ToleranceZoneAffectedPlane.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_Area.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_Centroid.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_ColorTool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_ColorType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_DataMapOfShapeLabel.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_DocumentTool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_Location.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_ShapeTool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_VisMaterial.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_VisMaterialCommon.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_VisMaterialPBR.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_VisMaterialTool.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFDoc_Volume.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFPrs_DataMapOfStyleTransient.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFPrs_DocumentNode.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFPrs_IndexedDataMapOfShapeStyle.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XCAFPrs_Style.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XSAlgo_ShapeProcessor.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XSControl_Controller.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\XSControl_Reader.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gce_ErrorType.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Ax1.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Ax2.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Ax2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Ax3.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Circ.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Cone.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Cylinder.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Dir.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Dir2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Elips.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_GTrsf.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Hypr.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Lin.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Lin2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Mat.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Mat2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Parab.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Pln.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Pnt.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Pnt2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Sphere.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Torus.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Trsf.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Trsf2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_TrsfForm.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Vec.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Vec2d.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Vec2f.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_Vec3f.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_VectorWithNullMagnitude.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_XY.hxx`
- `build\vcpkg_installed\x64-windows\include\opencascade\gp_XYZ.hxx`

### build\vcpkg_installed\x64-windows\include\pixman-1/

- `build\vcpkg_installed\x64-windows\include\pixman-1\pixman-version.h`
- `build\vcpkg_installed\x64-windows\include\pixman-1\pixman.h`

### build\vcpkg_installed\x64-windows\include\python3.11/

- `build\vcpkg_installed\x64-windows\include\python3.11\Python.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\abstract.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\bltinmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\boolobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\bytearrayobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\bytesobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\ceval.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\codecs.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\compile.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\complexobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\descrobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\dictobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\enumobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\exports.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\fileobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\fileutils.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\floatobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\frameobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\genericaliasobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\import.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\intrcheck.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\iterobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\listobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\longobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\memoryobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\methodobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\modsupport.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\moduleobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\object.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\objimpl.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\osmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\patchlevel.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pybuffer.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pycapsule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyconfig.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyerrors.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyframe.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyhash.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pylifecycle.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymacconfig.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymacro.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymath.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymem.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyport.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pystate.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pystrcmp.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pystrtod.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pythonrun.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pythread.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pytypedefs.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\rangeobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\setobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\sliceobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\structseq.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\sysmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\traceback.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\tracemalloc.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\tupleobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\typeslots.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\unicodeobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\warnings.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\weakrefobject.h`

### build\vcpkg_installed\x64-windows\include\python3.11\cpython/

- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\abstract.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\bytearrayobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\bytesobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\cellobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\ceval.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\classobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\code.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\compile.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\complexobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\context.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\descrobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\dictobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\fileobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\fileutils.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\floatobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\frameobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\funcobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\genobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\import.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\initconfig.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\listobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\longintrepr.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\longobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\methodobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\modsupport.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\object.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\objimpl.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\odictobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\picklebufobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyctype.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pydebug.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyerrors.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyfpe.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyframe.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pylifecycle.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pymem.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pystate.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pythonrun.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pythread.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pytime.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\setobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\sysmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\traceback.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\tupleobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\unicodeobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\warnings.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\weakrefobject.h`

### build\vcpkg_installed\x64-windows\include\wx/

- `build\vcpkg_installed\x64-windows\include\wx\aboutdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\accel.h`
- `build\vcpkg_installed\x64-windows\include\wx\access.h`
- `build\vcpkg_installed\x64-windows\include\wx\affinematrix2d.h`
- `build\vcpkg_installed\x64-windows\include\wx\affinematrix2dbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\afterstd.h`
- `build\vcpkg_installed\x64-windows\include\wx\any.h`
- `build\vcpkg_installed\x64-windows\include\wx\anybutton.h`
- `build\vcpkg_installed\x64-windows\include\wx\anystr.h`
- `build\vcpkg_installed\x64-windows\include\wx\app.h`
- `build\vcpkg_installed\x64-windows\include\wx\appprogress.h`
- `build\vcpkg_installed\x64-windows\include\wx\apptrait.h`
- `build\vcpkg_installed\x64-windows\include\wx\archive.h`
- `build\vcpkg_installed\x64-windows\include\wx\arrstr.h`
- `build\vcpkg_installed\x64-windows\include\wx\artprov.h`
- `build\vcpkg_installed\x64-windows\include\wx\atomic.h`
- `build\vcpkg_installed\x64-windows\include\wx\base64.h`
- `build\vcpkg_installed\x64-windows\include\wx\beforestd.h`
- `build\vcpkg_installed\x64-windows\include\wx\bitmap.h`
- `build\vcpkg_installed\x64-windows\include\wx\bmpbndl.h`
- `build\vcpkg_installed\x64-windows\include\wx\bmpbuttn.h`
- `build\vcpkg_installed\x64-windows\include\wx\bmpcbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\bookctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\brush.h`
- `build\vcpkg_installed\x64-windows\include\wx\buffer.h`
- `build\vcpkg_installed\x64-windows\include\wx\build.h`
- `build\vcpkg_installed\x64-windows\include\wx\button.h`
- `build\vcpkg_installed\x64-windows\include\wx\chartype.h`
- `build\vcpkg_installed\x64-windows\include\wx\checkbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\checkeddelete.h`
- `build\vcpkg_installed\x64-windows\include\wx\checklst.h`
- `build\vcpkg_installed\x64-windows\include\wx\chkconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\choicdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\choice.h`
- `build\vcpkg_installed\x64-windows\include\wx\choicebk.h`
- `build\vcpkg_installed\x64-windows\include\wx\clipbrd.h`
- `build\vcpkg_installed\x64-windows\include\wx\clntdata.h`
- `build\vcpkg_installed\x64-windows\include\wx\cmdargs.h`
- `build\vcpkg_installed\x64-windows\include\wx\cmdline.h`
- `build\vcpkg_installed\x64-windows\include\wx\cmndata.h`
- `build\vcpkg_installed\x64-windows\include\wx\collpane.h`
- `build\vcpkg_installed\x64-windows\include\wx\colour.h`
- `build\vcpkg_installed\x64-windows\include\wx\colourdata.h`
- `build\vcpkg_installed\x64-windows\include\wx\combo.h`
- `build\vcpkg_installed\x64-windows\include\wx\combobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\compiler.h`
- `build\vcpkg_installed\x64-windows\include\wx\compositewin.h`
- `build\vcpkg_installed\x64-windows\include\wx\confbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\config.h`
- `build\vcpkg_installed\x64-windows\include\wx\containr.h`
- `build\vcpkg_installed\x64-windows\include\wx\control.h`
- `build\vcpkg_installed\x64-windows\include\wx\convauto.h`
- `build\vcpkg_installed\x64-windows\include\wx\cpp.h`
- `build\vcpkg_installed\x64-windows\include\wx\crt.h`
- `build\vcpkg_installed\x64-windows\include\wx\ctrlsub.h`
- `build\vcpkg_installed\x64-windows\include\wx\cursor.h`
- `build\vcpkg_installed\x64-windows\include\wx\dataobj.h`
- `build\vcpkg_installed\x64-windows\include\wx\dataview.h`
- `build\vcpkg_installed\x64-windows\include\wx\datectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\dateevt.h`
- `build\vcpkg_installed\x64-windows\include\wx\datetime.h`
- `build\vcpkg_installed\x64-windows\include\wx\datetimectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\datstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\dc.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcbuffer.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcclient.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcgraph.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcmemory.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcprint.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcscreen.h`
- `build\vcpkg_installed\x64-windows\include\wx\debug.h`
- `build\vcpkg_installed\x64-windows\include\wx\defs.h`
- `build\vcpkg_installed\x64-windows\include\wx\dialog.h`
- `build\vcpkg_installed\x64-windows\include\wx\dir.h`
- `build\vcpkg_installed\x64-windows\include\wx\dirctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\dirdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\display.h`
- `build\vcpkg_installed\x64-windows\include\wx\dlimpexp.h`
- `build\vcpkg_installed\x64-windows\include\wx\dlist.h`
- `build\vcpkg_installed\x64-windows\include\wx\dnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\docview.h`
- `build\vcpkg_installed\x64-windows\include\wx\dvrenderers.h`
- `build\vcpkg_installed\x64-windows\include\wx\dynarray.h`
- `build\vcpkg_installed\x64-windows\include\wx\dynlib.h`
- `build\vcpkg_installed\x64-windows\include\wx\dynload.h`
- `build\vcpkg_installed\x64-windows\include\wx\editlbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\encconv.h`
- `build\vcpkg_installed\x64-windows\include\wx\event.h`
- `build\vcpkg_installed\x64-windows\include\wx\eventfilter.h`
- `build\vcpkg_installed\x64-windows\include\wx\evtloop.h`
- `build\vcpkg_installed\x64-windows\include\wx\except.h`
- `build\vcpkg_installed\x64-windows\include\wx\fdrepdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\features.h`
- `build\vcpkg_installed\x64-windows\include\wx\ffile.h`
- `build\vcpkg_installed\x64-windows\include\wx\file.h`
- `build\vcpkg_installed\x64-windows\include\wx\fileconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\filedlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\filedlgcustomize.h`
- `build\vcpkg_installed\x64-windows\include\wx\filefn.h`
- `build\vcpkg_installed\x64-windows\include\wx\filehistory.h`
- `build\vcpkg_installed\x64-windows\include\wx\filename.h`
- `build\vcpkg_installed\x64-windows\include\wx\filepicker.h`
- `build\vcpkg_installed\x64-windows\include\wx\filesys.h`
- `build\vcpkg_installed\x64-windows\include\wx\flags.h`
- `build\vcpkg_installed\x64-windows\include\wx\font.h`
- `build\vcpkg_installed\x64-windows\include\wx\fontenc.h`
- `build\vcpkg_installed\x64-windows\include\wx\fontenum.h`
- `build\vcpkg_installed\x64-windows\include\wx\frame.h`
- `build\vcpkg_installed\x64-windows\include\wx\fs_arc.h`
- `build\vcpkg_installed\x64-windows\include\wx\fs_zip.h`
- `build\vcpkg_installed\x64-windows\include\wx\fswatcher.h`
- `build\vcpkg_installed\x64-windows\include\wx\gauge.h`
- `build\vcpkg_installed\x64-windows\include\wx\gbsizer.h`
- `build\vcpkg_installed\x64-windows\include\wx\gdicmn.h`
- `build\vcpkg_installed\x64-windows\include\wx\gdiobj.h`
- `build\vcpkg_installed\x64-windows\include\wx\geometry.h`
- `build\vcpkg_installed\x64-windows\include\wx\glcanvas.h`
- `build\vcpkg_installed\x64-windows\include\wx\graphics.h`
- `build\vcpkg_installed\x64-windows\include\wx\grid.h`
- `build\vcpkg_installed\x64-windows\include\wx\hash.h`
- `build\vcpkg_installed\x64-windows\include\wx\hashmap.h`
- `build\vcpkg_installed\x64-windows\include\wx\hashset.h`
- `build\vcpkg_installed\x64-windows\include\wx\headercol.h`
- `build\vcpkg_installed\x64-windows\include\wx\headerctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\helpbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\htmllbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\hyperlink.h`
- `build\vcpkg_installed\x64-windows\include\wx\icon.h`
- `build\vcpkg_installed\x64-windows\include\wx\iconbndl.h`
- `build\vcpkg_installed\x64-windows\include\wx\iconloc.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\image.h`
- `build\vcpkg_installed\x64-windows\include\wx\imaggif.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagiff.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagjpeg.h`
- `build\vcpkg_installed\x64-windows\include\wx\imaglist.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagpcx.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagpng.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagpnm.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagtga.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagtiff.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagxpm.h`
- `build\vcpkg_installed\x64-windows\include\wx\infobar.h`
- `build\vcpkg_installed\x64-windows\include\wx\init.h`
- `build\vcpkg_installed\x64-windows\include\wx\intl.h`
- `build\vcpkg_installed\x64-windows\include\wx\iosfwrap.h`
- `build\vcpkg_installed\x64-windows\include\wx\ioswrap.h`
- `build\vcpkg_installed\x64-windows\include\wx\itemattr.h`
- `build\vcpkg_installed\x64-windows\include\wx\itemid.h`
- `build\vcpkg_installed\x64-windows\include\wx\kbdstate.h`
- `build\vcpkg_installed\x64-windows\include\wx\language.h`
- `build\vcpkg_installed\x64-windows\include\wx\layout.h`
- `build\vcpkg_installed\x64-windows\include\wx\list.h`
- `build\vcpkg_installed\x64-windows\include\wx\listbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\listbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\listctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\localedefs.h`
- `build\vcpkg_installed\x64-windows\include\wx\log.h`
- `build\vcpkg_installed\x64-windows\include\wx\longlong.h`
- `build\vcpkg_installed\x64-windows\include\wx\math.h`
- `build\vcpkg_installed\x64-windows\include\wx\mdi.h`
- `build\vcpkg_installed\x64-windows\include\wx\memory.h`
- `build\vcpkg_installed\x64-windows\include\wx\menu.h`
- `build\vcpkg_installed\x64-windows\include\wx\menuitem.h`
- `build\vcpkg_installed\x64-windows\include\wx\mimetype.h`
- `build\vcpkg_installed\x64-windows\include\wx\minifram.h`
- `build\vcpkg_installed\x64-windows\include\wx\module.h`
- `build\vcpkg_installed\x64-windows\include\wx\mousestate.h`
- `build\vcpkg_installed\x64-windows\include\wx\msgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msgout.h`
- `build\vcpkg_installed\x64-windows\include\wx\msgqueue.h`
- `build\vcpkg_installed\x64-windows\include\wx\mstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\nonownedwnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\notebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\numformatter.h`
- `build\vcpkg_installed\x64-windows\include\wx\object.h`
- `build\vcpkg_installed\x64-windows\include\wx\odcombo.h`
- `build\vcpkg_installed\x64-windows\include\wx\ownerdrw.h`
- `build\vcpkg_installed\x64-windows\include\wx\palette.h`
- `build\vcpkg_installed\x64-windows\include\wx\panel.h`
- `build\vcpkg_installed\x64-windows\include\wx\pen.h`
- `build\vcpkg_installed\x64-windows\include\wx\peninfobase.h`
- `build\vcpkg_installed\x64-windows\include\wx\pickerbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\platform.h`
- `build\vcpkg_installed\x64-windows\include\wx\platinfo.h`
- `build\vcpkg_installed\x64-windows\include\wx\popupwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\position.h`
- `build\vcpkg_installed\x64-windows\include\wx\print.h`
- `build\vcpkg_installed\x64-windows\include\wx\printdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\prntbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\process.h`
- `build\vcpkg_installed\x64-windows\include\wx\progdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\radiobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\radiobut.h`
- `build\vcpkg_installed\x64-windows\include\wx\range.h`
- `build\vcpkg_installed\x64-windows\include\wx\rawbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\rearrangectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\recguard.h`
- `build\vcpkg_installed\x64-windows\include\wx\regex.h`
- `build\vcpkg_installed\x64-windows\include\wx\region.h`
- `build\vcpkg_installed\x64-windows\include\wx\renderer.h`
- `build\vcpkg_installed\x64-windows\include\wx\richmsgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\rtti.h`
- `build\vcpkg_installed\x64-windows\include\wx\sckaddr.h`
- `build\vcpkg_installed\x64-windows\include\wx\scopedptr.h`
- `build\vcpkg_installed\x64-windows\include\wx\scopeguard.h`
- `build\vcpkg_installed\x64-windows\include\wx\scrolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\scrolwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\settings.h`
- `build\vcpkg_installed\x64-windows\include\wx\setup.h`
- `build\vcpkg_installed\x64-windows\include\wx\sharedptr.h`
- `build\vcpkg_installed\x64-windows\include\wx\simplebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\sizer.h`
- `build\vcpkg_installed\x64-windows\include\wx\slider.h`
- `build\vcpkg_installed\x64-windows\include\wx\snglinst.h`
- `build\vcpkg_installed\x64-windows\include\wx\socket.h`
- `build\vcpkg_installed\x64-windows\include\wx\spinbutt.h`
- `build\vcpkg_installed\x64-windows\include\wx\spinctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\splash.h`
- `build\vcpkg_installed\x64-windows\include\wx\splitter.h`
- `build\vcpkg_installed\x64-windows\include\wx\srchctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\sstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\statbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\statbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\statline.h`
- `build\vcpkg_installed\x64-windows\include\wx\stattext.h`
- `build\vcpkg_installed\x64-windows\include\wx\statusbr.h`
- `build\vcpkg_installed\x64-windows\include\wx\stdpaths.h`
- `build\vcpkg_installed\x64-windows\include\wx\stdstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\stockitem.h`
- `build\vcpkg_installed\x64-windows\include\wx\stopwatch.h`
- `build\vcpkg_installed\x64-windows\include\wx\strconv.h`
- `build\vcpkg_installed\x64-windows\include\wx\stream.h`
- `build\vcpkg_installed\x64-windows\include\wx\string.h`
- `build\vcpkg_installed\x64-windows\include\wx\stringimpl.h`
- `build\vcpkg_installed\x64-windows\include\wx\stringops.h`
- `build\vcpkg_installed\x64-windows\include\wx\strvararg.h`
- `build\vcpkg_installed\x64-windows\include\wx\sysopt.h`
- `build\vcpkg_installed\x64-windows\include\wx\systhemectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\tarstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\tbarbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\textbuf.h`
- `build\vcpkg_installed\x64-windows\include\wx\textctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\textdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\textentry.h`
- `build\vcpkg_installed\x64-windows\include\wx\textfile.h`
- `build\vcpkg_installed\x64-windows\include\wx\tglbtn.h`
- `build\vcpkg_installed\x64-windows\include\wx\thread.h`
- `build\vcpkg_installed\x64-windows\include\wx\time.h`
- `build\vcpkg_installed\x64-windows\include\wx\timer.h`
- `build\vcpkg_installed\x64-windows\include\wx\tokenzr.h`
- `build\vcpkg_installed\x64-windows\include\wx\toolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\tooltip.h`
- `build\vcpkg_installed\x64-windows\include\wx\toplevel.h`
- `build\vcpkg_installed\x64-windows\include\wx\tracker.h`
- `build\vcpkg_installed\x64-windows\include\wx\translation.h`
- `build\vcpkg_installed\x64-windows\include\wx\treebase.h`
- `build\vcpkg_installed\x64-windows\include\wx\treebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\treectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\treelist.h`
- `build\vcpkg_installed\x64-windows\include\wx\txtstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\typeinfo.h`
- `build\vcpkg_installed\x64-windows\include\wx\types.h`
- `build\vcpkg_installed\x64-windows\include\wx\unichar.h`
- `build\vcpkg_installed\x64-windows\include\wx\uri.h`
- `build\vcpkg_installed\x64-windows\include\wx\url.h`
- `build\vcpkg_installed\x64-windows\include\wx\utils.h`
- `build\vcpkg_installed\x64-windows\include\wx\valgen.h`
- `build\vcpkg_installed\x64-windows\include\wx\validate.h`
- `build\vcpkg_installed\x64-windows\include\wx\valnum.h`
- `build\vcpkg_installed\x64-windows\include\wx\valtext.h`
- `build\vcpkg_installed\x64-windows\include\wx\variant.h`
- `build\vcpkg_installed\x64-windows\include\wx\vector.h`
- `build\vcpkg_installed\x64-windows\include\wx\version.h`
- `build\vcpkg_installed\x64-windows\include\wx\versioninfo.h`
- `build\vcpkg_installed\x64-windows\include\wx\vidmode.h`
- `build\vcpkg_installed\x64-windows\include\wx\vlbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\vscroll.h`
- `build\vcpkg_installed\x64-windows\include\wx\weakref.h`
- `build\vcpkg_installed\x64-windows\include\wx\wfstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\window.h`
- `build\vcpkg_installed\x64-windows\include\wx\windowid.h`
- `build\vcpkg_installed\x64-windows\include\wx\withimages.h`
- `build\vcpkg_installed\x64-windows\include\wx\wupdlock.h`
- `build\vcpkg_installed\x64-windows\include\wx\wx.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxcrt.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxcrtbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxcrtvararg.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxhtml.h`
- `build\vcpkg_installed\x64-windows\include\wx\xti.h`
- `build\vcpkg_installed\x64-windows\include\wx\xti2.h`
- `build\vcpkg_installed\x64-windows\include\wx\zipstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\zstream.h`

### build\vcpkg_installed\x64-windows\include\wx\aui/

- `build\vcpkg_installed\x64-windows\include\wx\aui\aui.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\auibar.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\auibook.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\barartmsw.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\dockart.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\floatpane.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\framemanager.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\tabart.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\tabartmsw.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\tabmdi.h`

### build\vcpkg_installed\x64-windows\include\wx\generic/

- `build\vcpkg_installed\x64-windows\include\wx\generic\choicdgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\collpaneg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\combo.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dataview.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dirctrlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dvrenderer.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dvrenderers.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\filepickerg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\grid.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\gridctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\grideditors.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\hyperlink.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\infobar.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\logg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\msgdlgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\progdlgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\richmsgdlgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\scrolwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\spinctlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\splash.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\splitter.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\srchctlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\textdlgg.h`

### build\vcpkg_installed\x64-windows\include\wx\html/

- `build\vcpkg_installed\x64-windows\include\wx\html\helpctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\helpdata.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\helpfrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\helpwnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlcell.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmldefs.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlfilt.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlpars.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmltag.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmprint.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\m_templ.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\styleparams.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\winpars.h`

### build\vcpkg_installed\x64-windows\include\wx\meta/

- `build\vcpkg_installed\x64-windows\include\wx\meta\convertible.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\if.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\implicitconversion.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\int2type.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\movable.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\pod.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\removeref.h`

### build\vcpkg_installed\x64-windows\include\wx\msw/

- `build\vcpkg_installed\x64-windows\include\wx\msw\accel.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\anybutton.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\app.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\appprogress.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\apptbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\apptrait.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\bitmap.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\bmpbuttn.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\bmpcbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\brush.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\button.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\checkbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\checklst.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\chkconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\choice.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\clipbrd.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\colour.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\combo.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\combobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\control.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ctrlsub.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\cursor.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\datectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\datetimectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\dialog.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\dirdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\enhmeta.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\evtloop.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\evtloopconsole.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\fdrepdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\filedlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\font.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\frame.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\fswatcher.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\gauge.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\gdiimage.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\glcanvas.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\headerctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\hyperlink.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\icon.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\imaglist.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\init.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\libraries.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\listbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\listctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\mdi.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\menu.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\menuitem.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\minifram.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\msgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\nonownedwnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\notebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ownerdrawnbutton.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ownerdrw.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\palette.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\panel.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\pen.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\popupwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\printwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\progdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\radiobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\radiobut.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\regconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\region.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\registry.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\richmsgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\scrolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\slider.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\spinbutt.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\spinctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statline.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\stattext.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statusbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\stdpaths.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\textctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\textentry.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\tglbtn.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\toolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\tooltip.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\toplevel.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\treectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\window.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\winundef.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\winver.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\wrapwin.h`

### build\vcpkg_installed\x64-windows\include\wx\msw\ole/

- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\access.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dataform.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dataobj.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dataobj2.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dropsrc.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\droptgt.h`

### build\vcpkg_installed\x64-windows\include\wx\propgrid/

- `build\vcpkg_installed\x64-windows\include\wx\propgrid\advprops.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\editors.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\manager.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\property.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgrid.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgriddefs.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgridiface.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgridpagestate.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\props.h`

### build\vcpkg_installed\x64-windows\include\wx\protocol/

- `build\vcpkg_installed\x64-windows\include\wx\protocol\http.h`
- `build\vcpkg_installed\x64-windows\include\wx\protocol\protocol.h`

### build\vcpkg_installed\x64-windows\include\wx\stc/

- `build\vcpkg_installed\x64-windows\include\wx\stc\stc.h`

### build\vcpkg_installed\x64-windows\include\wx\xml/

- `build\vcpkg_installed\x64-windows\include\wx\xml\xml.h`

### build\vcpkg_installed\x64-windows\include\wx\xrc/

- `build\vcpkg_installed\x64-windows\include\wx\xrc\xmlres.h`
- `build\vcpkg_installed\x64-windows\include\wx\xrc\xmlreshandler.h`

### common/

- `common\design_block.h`
- `common\design_block_info.h`
- `common\design_block_info_impl.h`
- `common\design_block_io.h`

### common\dialog_about/

- `common\dialog_about\aboutinfo.h`
- `common\dialog_about\dialog_about.h`
- `common\dialog_about\dialog_about_base.h`

### common\dialogs/

- `common\dialogs\dialog_HTML_reporter_base.h`
- `common\dialogs\dialog_assign_netclass_base.h`
- `common\dialogs\dialog_book_reporter_base.h`
- `common\dialogs\dialog_color_picker_base.h`
- `common\dialogs\dialog_configure_paths_base.h`
- `common\dialogs\dialog_display_html_text_base.h`
- `common\dialogs\dialog_global_lib_table_config_base.h`
- `common\dialogs\dialog_grid_settings_base.h`
- `common\dialogs\dialog_import_choose_project.h`
- `common\dialogs\dialog_import_choose_project_base.h`
- `common\dialogs\dialog_locked_items_query_base.h`
- `common\dialogs\dialog_migrate_settings_base.h`
- `common\dialogs\dialog_page_settings_base.h`
- `common\dialogs\dialog_paste_special_base.h`
- `common\dialogs\dialog_plugin_options_base.h`
- `common\dialogs\dialog_print_generic_base.h`
- `common\dialogs\dialog_rc_job.h`
- `common\dialogs\dialog_rc_job_base.h`
- `common\dialogs\dialog_text_entry_base.h`
- `common\dialogs\dialog_unit_entry_base.h`
- `common\dialogs\eda_list_dialog_base.h`
- `common\dialogs\eda_reorderable_list_dialog_base.h`
- `common\dialogs\eda_view_switcher_base.h`
- `common\dialogs\hotkey_cycle_popup.h`
- `common\dialogs\panel_color_settings_base.h`
- `common\dialogs\panel_common_settings_base.h`
- `common\dialogs\panel_data_collection_base.h`
- `common\dialogs\panel_embedded_files.h`
- `common\dialogs\panel_embedded_files_base.h`
- `common\dialogs\panel_grid_settings_base.h`
- `common\dialogs\panel_image_editor_base.h`
- `common\dialogs\panel_mouse_settings_base.h`
- `common\dialogs\panel_packages_and_updates.h`
- `common\dialogs\panel_packages_and_updates_base.h`
- `common\dialogs\panel_plugin_settings_base.h`
- `common\dialogs\panel_setup_netclasses_base.h`
- `common\dialogs\panel_text_variables_base.h`

### common\dialogs\git/

- `common\dialogs\git\dialog_git_repository.h`
- `common\dialogs\git\dialog_git_repository_base.h`
- `common\dialogs\git\panel_git_repos.h`
- `common\dialogs\git\panel_git_repos_base.h`

### common\gal\opengl/

- `common\gal\opengl\SmaaAreaTex.h`
- `common\gal\opengl\SmaaSearchTex.h`
- `common\gal\opengl\antialiasing.h`
- `common\gal\opengl\gl_resources.h`

### common\git/

- `common\git\kicad_git_common.h`
- `common\git\kicad_git_errors.h`
- `common\git\kicad_git_memory.h`

### common\import_gfx/

- `common\import_gfx\dxf_import_plugin.h`
- `common\import_gfx\graphics_import_mgr.h`
- `common\import_gfx\graphics_import_plugin.h`
- `common\import_gfx\graphics_importer.h`
- `common\import_gfx\graphics_importer_buffer.h`
- `common\import_gfx\svg_import_plugin.h`

### common\io/

- `common\io\io_utils.h`

### common\io\altium/

- `common\io\altium\altium_ascii_parser.h`
- `common\io\altium\altium_binary_parser.h`
- `common\io\altium\altium_parser_utils.h`
- `common\io\altium\altium_props_utils.h`

### common\io\cadstar/

- `common\io\cadstar\cadstar_archive_objects.h`
- `common\io\cadstar\cadstar_archive_parser.h`
- `common\io\cadstar\cadstar_parts_lib_grammar.h`
- `common\io\cadstar\cadstar_parts_lib_model.h`
- `common\io\cadstar\cadstar_parts_lib_parser.h`

### common\io\common/

- `common\io\common\plugin_common_choose_project.h`

### common\io\eagle/

- `common\io\eagle\eagle_parser.h`

### common\io\easyeda/

- `common\io\easyeda\easyeda_parser_base.h`
- `common\io\easyeda\easyeda_parser_structs.h`

### common\io\easyedapro/

- `common\io\easyedapro\easyedapro_import_utils.h`
- `common\io\easyedapro\easyedapro_parser.h`

### common\jobs/

- `common\jobs\job.h`
- `common\jobs\job_dispatcher.h`
- `common\jobs\job_export_pcb_3d.h`
- `common\jobs\job_export_pcb_drill.h`
- `common\jobs\job_export_pcb_dxf.h`
- `common\jobs\job_export_pcb_gencad.h`
- `common\jobs\job_export_pcb_gerber.h`
- `common\jobs\job_export_pcb_gerbers.h`
- `common\jobs\job_export_pcb_ipc2581.h`
- `common\jobs\job_export_pcb_ipcd356.h`
- `common\jobs\job_export_pcb_odb.h`
- `common\jobs\job_export_pcb_pdf.h`
- `common\jobs\job_export_pcb_plot.h`
- `common\jobs\job_export_pcb_pos.h`
- `common\jobs\job_export_pcb_svg.h`
- `common\jobs\job_export_sch_bom.h`
- `common\jobs\job_export_sch_netlist.h`
- `common\jobs\job_export_sch_plot.h`
- `common\jobs\job_fp_export_svg.h`
- `common\jobs\job_fp_upgrade.h`
- `common\jobs\job_pcb_drc.h`
- `common\jobs\job_pcb_render.h`
- `common\jobs\job_rc.h`

### common\settings/

- `common\settings\builtin_color_themes.h`

### common\tool/

- `common\tool\library_editor_control.h`

### common\widgets/

- `common\widgets\gal_options_panel_base.h`
- `common\widgets\properties_panel.h`
- `common\widgets\search_pane_base.h`
- `common\widgets\wx_html_report_box.h`
- `common\widgets\wx_html_report_panel.h`
- `common\widgets\wx_html_report_panel_base.h`

### cvpcb/

- `cvpcb\auto_associate.h`
- `cvpcb\cvpcb_association.h`
- `cvpcb\cvpcb_id.h`
- `cvpcb\cvpcb_mainframe.h`
- `cvpcb\display_footprints_frame.h`
- `cvpcb\listboxes.h`

### cvpcb\dialogs/

- `cvpcb\dialogs\dialog_config_equfiles.h`
- `cvpcb\dialogs\dialog_config_equfiles_base.h`
- `cvpcb\dialogs\fp_conflict_assignment_selector.h`
- `cvpcb\dialogs\fp_conflict_assignment_selector_base.h`

### cvpcb\tools/

- `cvpcb\tools\cvpcb_actions.h`
- `cvpcb\tools\cvpcb_association_tool.h`
- `cvpcb\tools\cvpcb_control.h`
- `cvpcb\tools\cvpcb_fpviewer_selection_tool.h`

### eeschema/

- `eeschema\bom_plugins.h`
- `eeschema\bus_alias.h`
- `eeschema\connection_graph.h`
- `eeschema\default_values.h`
- `eeschema\design_block_tree_model_adapter.h`
- `eeschema\eeschema_helpers.h`
- `eeschema\eeschema_id.h`
- `eeschema\eeschema_settings.h`
- `eeschema\fields_data_model.h`
- `eeschema\fields_grid_table.h`
- `eeschema\general.h`
- `eeschema\generate_alias_info.h`
- `eeschema\gfx_import_utils.h`
- `eeschema\invoke_sch_dialog.h`
- `eeschema\junction_helpers.h`
- `eeschema\lib_symbol.h`
- `eeschema\pin_layout_cache.h`
- `eeschema\pin_numbers.h`
- `eeschema\pin_type.h`
- `eeschema\project_rescue.h`
- `eeschema\project_sch.h`
- `eeschema\sch_base_frame.h`
- `eeschema\sch_bitmap.h`
- `eeschema\sch_bus_entry.h`
- `eeschema\sch_collectors.h`
- `eeschema\sch_commit.h`
- `eeschema\sch_connection.h`
- `eeschema\sch_draw_panel.h`
- `eeschema\sch_edit_frame.h`
- `eeschema\sch_field.h`
- `eeschema\sch_file_versions.h`
- `eeschema\sch_item.h`
- `eeschema\sch_junction.h`
- `eeschema\sch_label.h`
- `eeschema\sch_line.h`
- `eeschema\sch_marker.h`
- `eeschema\sch_no_connect.h`
- `eeschema\sch_painter.h`
- `eeschema\sch_pin.h`
- `eeschema\sch_plotter.h`
- `eeschema\sch_preview_panel.h`
- `eeschema\sch_reference_list.h`
- `eeschema\sch_render_settings.h`
- `eeschema\sch_rtree.h`
- `eeschema\sch_rule_area.h`
- `eeschema\sch_screen.h`
- `eeschema\sch_shape.h`
- `eeschema\sch_sheet.h`
- `eeschema\sch_sheet_path.h`
- `eeschema\sch_sheet_pin.h`
- `eeschema\sch_symbol.h`
- `eeschema\sch_table.h`
- `eeschema\sch_tablecell.h`
- `eeschema\sch_text.h`
- `eeschema\sch_text_help_md.h`
- `eeschema\sch_textbox.h`
- `eeschema\sch_validators.h`
- `eeschema\sch_view.h`
- `eeschema\schematic.h`
- `eeschema\schematic_holder.h`
- `eeschema\schematic_settings.h`
- `eeschema\symb_transforms_utils.h`
- `eeschema\symbol.h`
- `eeschema\symbol_async_loader.h`
- `eeschema\symbol_chooser_frame.h`
- `eeschema\symbol_lib_table.h`
- `eeschema\symbol_library.h`
- `eeschema\symbol_library_common.h`
- `eeschema\symbol_library_manager.h`
- `eeschema\symbol_tree_model_adapter.h`
- `eeschema\symbol_tree_synchronizing_adapter.h`
- `eeschema\symbol_viewer_frame.h`

### eeschema\api/

- `eeschema\api\api_handler_sch.h`
- `eeschema\api\api_sch_utils.h`

### eeschema\dialogs/

- `eeschema\dialogs\dialog_annotate_base.h`
- `eeschema\dialogs\dialog_bom.h`
- `eeschema\dialogs\dialog_bom_base.h`
- `eeschema\dialogs\dialog_bom_help_md.h`
- `eeschema\dialogs\dialog_change_symbols.h`
- `eeschema\dialogs\dialog_change_symbols_base.h`
- `eeschema\dialogs\dialog_database_lib_settings.h`
- `eeschema\dialogs\dialog_database_lib_settings_base.h`
- `eeschema\dialogs\dialog_design_block_properties.h`
- `eeschema\dialogs\dialog_design_block_properties_base.h`
- `eeschema\dialogs\dialog_edit_symbols_libid_base.h`
- `eeschema\dialogs\dialog_eeschema_page_settings.h`
- `eeschema\dialogs\dialog_erc.h`
- `eeschema\dialogs\dialog_erc_base.h`
- `eeschema\dialogs\dialog_export_netlist.h`
- `eeschema\dialogs\dialog_export_netlist_base.h`
- `eeschema\dialogs\dialog_field_properties.h`
- `eeschema\dialogs\dialog_field_properties_base.h`
- `eeschema\dialogs\dialog_global_edit_text_and_graphics_base.h`
- `eeschema\dialogs\dialog_global_sym_lib_table_config.h`
- `eeschema\dialogs\dialog_ibis_parser_reporter.h`
- `eeschema\dialogs\dialog_ibis_parser_reporter_base.h`
- `eeschema\dialogs\dialog_image_properties.h`
- `eeschema\dialogs\dialog_image_properties_base.h`
- `eeschema\dialogs\dialog_increment_annotations_base.h`
- `eeschema\dialogs\dialog_junction_props.h`
- `eeschema\dialogs\dialog_junction_props_base.h`
- `eeschema\dialogs\dialog_label_properties.h`
- `eeschema\dialogs\dialog_label_properties_base.h`
- `eeschema\dialogs\dialog_lib_edit_pin_table.h`
- `eeschema\dialogs\dialog_lib_edit_pin_table_base.h`
- `eeschema\dialogs\dialog_lib_new_symbol.h`
- `eeschema\dialogs\dialog_lib_new_symbol_base.h`
- `eeschema\dialogs\dialog_lib_symbol_properties.h`
- `eeschema\dialogs\dialog_lib_symbol_properties_base.h`
- `eeschema\dialogs\dialog_line_properties.h`
- `eeschema\dialogs\dialog_line_properties_base.h`
- `eeschema\dialogs\dialog_migrate_buses.h`
- `eeschema\dialogs\dialog_migrate_buses_base.h`
- `eeschema\dialogs\dialog_pin_properties.h`
- `eeschema\dialogs\dialog_pin_properties_base.h`
- `eeschema\dialogs\dialog_plot_schematic.h`
- `eeschema\dialogs\dialog_plot_schematic_base.h`
- `eeschema\dialogs\dialog_rescue_each_base.h`
- `eeschema\dialogs\dialog_sch_import_settings.h`
- `eeschema\dialogs\dialog_sch_import_settings_base.h`
- `eeschema\dialogs\dialog_schematic_find.h`
- `eeschema\dialogs\dialog_schematic_find_base.h`
- `eeschema\dialogs\dialog_schematic_setup.h`
- `eeschema\dialogs\dialog_shape_properties.h`
- `eeschema\dialogs\dialog_shape_properties_base.h`
- `eeschema\dialogs\dialog_sheet_pin_properties.h`
- `eeschema\dialogs\dialog_sheet_pin_properties_base.h`
- `eeschema\dialogs\dialog_sheet_properties.h`
- `eeschema\dialogs\dialog_sheet_properties_base.h`
- `eeschema\dialogs\dialog_sim_command.h`
- `eeschema\dialogs\dialog_sim_command_base.h`
- `eeschema\dialogs\dialog_sim_format_value.h`
- `eeschema\dialogs\dialog_sim_format_value_base.h`
- `eeschema\dialogs\dialog_sim_model.h`
- `eeschema\dialogs\dialog_sim_model_base.h`
- `eeschema\dialogs\dialog_symbol_chooser.h`
- `eeschema\dialogs\dialog_symbol_fields_table.h`
- `eeschema\dialogs\dialog_symbol_fields_table_base.h`
- `eeschema\dialogs\dialog_symbol_properties.h`
- `eeschema\dialogs\dialog_symbol_properties_base.h`
- `eeschema\dialogs\dialog_symbol_remap.h`
- `eeschema\dialogs\dialog_symbol_remap_base.h`
- `eeschema\dialogs\dialog_table_properties.h`
- `eeschema\dialogs\dialog_table_properties_base.h`
- `eeschema\dialogs\dialog_tablecell_properties.h`
- `eeschema\dialogs\dialog_tablecell_properties_base.h`
- `eeschema\dialogs\dialog_text_properties.h`
- `eeschema\dialogs\dialog_text_properties_base.h`
- `eeschema\dialogs\dialog_update_from_pcb.h`
- `eeschema\dialogs\dialog_update_from_pcb_base.h`
- `eeschema\dialogs\dialog_update_symbol_fields.h`
- `eeschema\dialogs\dialog_update_symbol_fields_base.h`
- `eeschema\dialogs\dialog_user_defined_signals.h`
- `eeschema\dialogs\dialog_user_defined_signals_base.h`
- `eeschema\dialogs\dialog_wire_bus_properties.h`
- `eeschema\dialogs\dialog_wire_bus_properties_base.h`
- `eeschema\dialogs\panel_bom_presets.h`
- `eeschema\dialogs\panel_bom_presets_base.h`
- `eeschema\dialogs\panel_eeschema_color_settings.h`
- `eeschema\dialogs\panel_setup_buses.h`
- `eeschema\dialogs\panel_setup_buses_base.h`
- `eeschema\dialogs\panel_setup_formatting.h`
- `eeschema\dialogs\panel_setup_formatting_base.h`
- `eeschema\dialogs\panel_setup_pinmap.h`
- `eeschema\dialogs\panel_setup_pinmap_base.h`
- `eeschema\dialogs\panel_sym_lib_table.h`
- `eeschema\dialogs\panel_sym_lib_table_base.h`
- `eeschema\dialogs\panel_template_fieldnames.h`
- `eeschema\dialogs\panel_template_fieldnames_base.h`

### eeschema\erc/

- `eeschema\erc\erc.h`
- `eeschema\erc\erc_item.h`
- `eeschema\erc\erc_report.h`
- `eeschema\erc\erc_sch_pin_context.h`
- `eeschema\erc\erc_settings.h`

### eeschema\import_gfx/

- `eeschema\import_gfx\dialog_import_gfx_sch.h`
- `eeschema\import_gfx\dialog_import_gfx_sch_base.h`
- `eeschema\import_gfx\graphics_importer_lib_symbol.h`
- `eeschema\import_gfx\graphics_importer_sch.h`

### eeschema\navlib/

- `eeschema\navlib\nl_schematic_plugin.h`
- `eeschema\navlib\nl_schematic_plugin_impl.h`

### eeschema\netlist_exporters/

- `eeschema\netlist_exporters\netlist.h`
- `eeschema\netlist_exporters\netlist_exporter_allegro.h`
- `eeschema\netlist_exporters\netlist_exporter_base.h`
- `eeschema\netlist_exporters\netlist_exporter_cadstar.h`
- `eeschema\netlist_exporters\netlist_exporter_kicad.h`
- `eeschema\netlist_exporters\netlist_exporter_orcadpcb2.h`
- `eeschema\netlist_exporters\netlist_exporter_pads.h`
- `eeschema\netlist_exporters\netlist_exporter_spice.h`
- `eeschema\netlist_exporters\netlist_exporter_spice_model.h`
- `eeschema\netlist_exporters\netlist_exporter_xml.h`

### eeschema\printing/

- `eeschema\printing\dialog_print.h`
- `eeschema\printing\dialog_print_base.h`
- `eeschema\printing\sch_printout.h`

### eeschema\sch_io/

- `eeschema\sch_io\sch_io.h`
- `eeschema\sch_io\sch_io_lib_cache.h`
- `eeschema\sch_io\sch_io_mgr.h`

### eeschema\sch_io\altium/

- `eeschema\sch_io\altium\altium_parser_sch.h`
- `eeschema\sch_io\altium\sch_io_altium.h`

### eeschema\sch_io\cadstar/

- `eeschema\sch_io\cadstar\cadstar_sch_archive_loader.h`
- `eeschema\sch_io\cadstar\cadstar_sch_archive_parser.h`
- `eeschema\sch_io\cadstar\sch_io_cadstar_archive.h`

### eeschema\sch_io\database/

- `eeschema\sch_io\database\sch_io_database.h`

### eeschema\sch_io\eagle/

- `eeschema\sch_io\eagle\sch_io_eagle.h`

### eeschema\sch_io\easyeda/

- `eeschema\sch_io\easyeda\sch_easyeda_parser.h`
- `eeschema\sch_io\easyeda\sch_io_easyeda.h`

### eeschema\sch_io\easyedapro/

- `eeschema\sch_io\easyedapro\sch_easyedapro_parser.h`
- `eeschema\sch_io\easyedapro\sch_io_easyedapro.h`

### eeschema\sch_io\http_lib/

- `eeschema\sch_io\http_lib\sch_io_http_lib.h`

### eeschema\sch_io\kicad_legacy/

- `eeschema\sch_io\kicad_legacy\sch_io_kicad_legacy.h`
- `eeschema\sch_io\kicad_legacy\sch_io_kicad_legacy_helpers.h`
- `eeschema\sch_io\kicad_legacy\sch_io_kicad_legacy_lib_cache.h`

### eeschema\sch_io\kicad_sexpr/

- `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr.h`
- `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr_common.h`
- `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr_lib_cache.h`
- `eeschema\sch_io\kicad_sexpr\sch_io_kicad_sexpr_parser.h`

### eeschema\sch_io\ltspice/

- `eeschema\sch_io\ltspice\ltspice_schematic.h`
- `eeschema\sch_io\ltspice\sch_io_ltspice.h`
- `eeschema\sch_io\ltspice\sch_io_ltspice_parser.h`

### eeschema\sim/

- `eeschema\sim\ngspice.h`
- `eeschema\sim\sim_lib_mgr.h`
- `eeschema\sim\sim_library.h`
- `eeschema\sim\sim_library_ibis.h`
- `eeschema\sim\sim_library_spice.h`
- `eeschema\sim\sim_model.h`
- `eeschema\sim\sim_model_behavioral.h`
- `eeschema\sim\sim_model_ibis.h`
- `eeschema\sim\sim_model_ideal.h`
- `eeschema\sim\sim_model_l_mutual.h`
- `eeschema\sim\sim_model_ngspice.h`
- `eeschema\sim\sim_model_r_pot.h`
- `eeschema\sim\sim_model_raw_spice.h`
- `eeschema\sim\sim_model_serializer.h`
- `eeschema\sim\sim_model_source.h`
- `eeschema\sim\sim_model_spice.h`
- `eeschema\sim\sim_model_spice_fallback.h`
- `eeschema\sim\sim_model_subckt.h`
- `eeschema\sim\sim_model_switch.h`
- `eeschema\sim\sim_model_tline.h`
- `eeschema\sim\sim_model_xspice.h`
- `eeschema\sim\sim_plot_colors.h`
- `eeschema\sim\sim_plot_tab.h`
- `eeschema\sim\sim_preferences.h`
- `eeschema\sim\sim_property.h`
- `eeschema\sim\sim_tab.h`
- `eeschema\sim\sim_types.h`
- `eeschema\sim\sim_value.h`
- `eeschema\sim\sim_xspice_parser.h`
- `eeschema\sim\simulator.h`
- `eeschema\sim\simulator_frame.h`
- `eeschema\sim\simulator_frame_ui.h`
- `eeschema\sim\simulator_frame_ui_base.h`
- `eeschema\sim\simulator_reporter.h`
- `eeschema\sim\spice_circuit_model.h`
- `eeschema\sim\spice_generator.h`
- `eeschema\sim\spice_grammar.h`
- `eeschema\sim\spice_library_parser.h`
- `eeschema\sim\spice_model_parser.h`
- `eeschema\sim\spice_settings.h`
- `eeschema\sim\spice_simulator.h`
- `eeschema\sim\spice_value.h`
- `eeschema\sim\user_defined_signals_help_md.h`

### eeschema\sim\kibis/

- `eeschema\sim\kibis\ibis_parser.h`
- `eeschema\sim\kibis\kibis.h`

### eeschema\symbol_editor/

- `eeschema\symbol_editor\lib_logger.h`
- `eeschema\symbol_editor\lib_symbol_library_manager.h`
- `eeschema\symbol_editor\symbol_edit_frame.h`
- `eeschema\symbol_editor\symbol_editor_settings.h`
- `eeschema\symbol_editor\symbol_saveas_type.h`

### eeschema\sync_sheet_pin/

- `eeschema\sync_sheet_pin\dialog_sync_sheet_pins.h`
- `eeschema\sync_sheet_pin\dialog_sync_sheet_pins_base.h`
- `eeschema\sync_sheet_pin\panel_sync_sheet_pins.h`
- `eeschema\sync_sheet_pin\panel_sync_sheet_pins_base.h`
- `eeschema\sync_sheet_pin\sheet_synchronization_agent.h`
- `eeschema\sync_sheet_pin\sheet_synchronization_item.h`
- `eeschema\sync_sheet_pin\sheet_synchronization_model.h`
- `eeschema\sync_sheet_pin\sheet_synchronization_notifier.h`
- `eeschema\sync_sheet_pin\sync_sheet_pin_preference.h`

### eeschema\tools/

- `eeschema\tools\backannotate.h`
- `eeschema\tools\ee_grid_helper.h`
- `eeschema\tools\rule_area_create_helper.h`
- `eeschema\tools\sch_actions.h`
- `eeschema\tools\sch_design_block_control.h`
- `eeschema\tools\sch_drawing_tools.h`
- `eeschema\tools\sch_edit_table_tool.h`
- `eeschema\tools\sch_edit_tool.h`
- `eeschema\tools\sch_editor_conditions.h`
- `eeschema\tools\sch_editor_control.h`
- `eeschema\tools\sch_find_replace_tool.h`
- `eeschema\tools\sch_inspection_tool.h`
- `eeschema\tools\sch_line_wire_bus_tool.h`
- `eeschema\tools\sch_move_tool.h`
- `eeschema\tools\sch_navigate_tool.h`
- `eeschema\tools\sch_point_editor.h`
- `eeschema\tools\sch_selection.h`
- `eeschema\tools\sch_selection_tool.h`
- `eeschema\tools\sch_tool_base.h`
- `eeschema\tools\sch_tool_utils.h`
- `eeschema\tools\simulator_control.h`
- `eeschema\tools\symbol_editor_control.h`
- `eeschema\tools\symbol_editor_drawing_tools.h`
- `eeschema\tools\symbol_editor_edit_tool.h`
- `eeschema\tools\symbol_editor_move_tool.h`
- `eeschema\tools\symbol_editor_pin_tool.h`

### eeschema\widgets/

- `eeschema\widgets\design_block_pane.h`
- `eeschema\widgets\design_block_preview_widget.h`
- `eeschema\widgets\filedlg_hook_save_project.h`
- `eeschema\widgets\hierarchy_pane.h`
- `eeschema\widgets\panel_design_block_chooser.h`
- `eeschema\widgets\panel_sch_selection_filter.h`
- `eeschema\widgets\panel_sch_selection_filter_base.h`
- `eeschema\widgets\panel_symbol_chooser.h`
- `eeschema\widgets\pin_shape_combobox.h`
- `eeschema\widgets\pin_type_combobox.h`
- `eeschema\widgets\sch_properties_panel.h`
- `eeschema\widgets\sch_search_pane.h`
- `eeschema\widgets\search_handlers.h`
- `eeschema\widgets\symbol_diff_widget.h`
- `eeschema\widgets\symbol_filedlg_save_as.h`
- `eeschema\widgets\symbol_filter_combobox.h`
- `eeschema\widgets\symbol_preview_widget.h`
- `eeschema\widgets\symbol_tree_pane.h`
- `eeschema\widgets\tuner_slider.h`
- `eeschema\widgets\tuner_slider_base.h`

### include/

- `include\3d_enums.h`
- `include\advanced_config.h`
- `include\array_axis.h`
- `include\array_options.h`
- `include\asset_archive.h`
- `include\background_jobs_monitor.h`
- `include\base_screen.h`
- `include\base_set.h`
- `include\base_units.h`
- `include\bin_mod.h`
- `include\bitmap_base.h`
- `include\bitmap_store.h`
- `include\bitmaps.h`
- `include\board_design_settings.h`
- `include\board_item.h`
- `include\board_printout.h`
- `include\build_version.h`
- `include\callback_gal.h`
- `include\case_insensitive_map.h`
- `include\class_draw_panel_gal.h`
- `include\cli_progress_reporter.h`
- `include\clipboard.h`
- `include\collector.h`
- `include\commit.h`
- `include\common.h`
- `include\config_map.h`
- `include\config_params.h`
- `include\confirm.h`
- `include\ctl_flags.h`
- `include\design_block_lib_table.h`
- `include\dialog_shim.h`
- `include\dpi_scaling.h`
- `include\dpi_scaling_common.h`
- `include\dsnlexer.h`
- `include\eda_base_frame.h`
- `include\eda_dde.h`
- `include\eda_doc.h`
- `include\eda_draw_frame.h`
- `include\eda_item.h`
- `include\eda_item_flags.h`
- `include\eda_list_dialog.h`
- `include\eda_pattern_match.h`
- `include\eda_search_data.h`
- `include\eda_shape.h`
- `include\eda_text.h`
- `include\eda_units.h`
- `include\embedded_files.h`
- `include\embedded_files_parser.h`
- `include\enum_vector.h`
- `include\env_paths.h`
- `include\env_vars.h`
- `include\executable_names.h`
- `include\file_history.h`
- `include\filename_resolver.h`
- `include\filter_reader.h`
- `include\fmt.h`
- `include\footprint_editor_settings.h`
- `include\footprint_filter.h`
- `include\footprint_info.h`
- `include\fp_lib_table.h`
- `include\frame_type.h`
- `include\gal_display_options_common.h`
- `include\gbr_metadata.h`
- `include\gbr_netlist_metadata.h`
- `include\gestfich.h`
- `include\gr_basic.h`
- `include\gr_text.h`
- `include\grid_tricks.h`
- `include\hash_eda.h`
- `include\hashtables.h`
- `include\hotkey_store.h`
- `include\hotkeys_basic.h`
- `include\i18n_utility.h`
- `include\id.h`
- `include\import_export.h`
- `include\increment.h`
- `include\inspectable.h`
- `include\json_common.h`
- `include\json_conversions.h`
- `include\json_schema_validator.h`
- `include\ki_any.h`
- `include\ki_exception.h`
- `include\kicommon.h`
- `include\kidialog.h`
- `include\kiface_base.h`
- `include\kiface_ids.h`
- `include\kiid.h`
- `include\kiway.h`
- `include\kiway_express.h`
- `include\kiway_holder.h`
- `include\kiway_player.h`
- `include\launch_ext.h`
- `include\layer_ids.h`
- `include\layer_range.h`
- `include\lib_id.h`
- `include\lib_table_base.h`
- `include\lib_table_grid.h`
- `include\lib_table_grid_tricks.h`
- `include\lib_tree_item.h`
- `include\lib_tree_model.h`
- `include\lib_tree_model_adapter.h`
- `include\locale_io.h`
- `include\lockfile.h`
- `include\logging.h`
- `include\lseq.h`
- `include\lset.h`
- `include\macros.h`
- `include\macros_swig.h`
- `include\mail_type.h`
- `include\marker_base.h`
- `include\markup_parser.h`
- `include\netclass.h`
- `include\newstroke_font.h`
- `include\notifications_manager.h`
- `include\origin_transforms.h`
- `include\origin_viewitem.h`
- `include\outline_mode.h`
- `include\page_info.h`
- `include\panel_hotkeys_editor.h`
- `include\panel_text_variables.h`
- `include\paths.h`
- `include\pcb_base_frame.h`
- `include\pcb_display_options.h`
- `include\pcb_screen.h`
- `include\pgm_base.h`
- `include\policy_keys.h`
- `include\printout.h`
- `include\progress_reporter.h`
- `include\project.h`
- `include\project_pcb.h`
- `include\ptree.h`
- `include\rc_item.h`
- `include\rc_json_schema.h`
- `include\refdes_utils.h`
- `include\reference_image.h`
- `include\render_settings.h`
- `include\reporter.h`
- `include\richio.h`
- `include\scintilla_tricks.h`
- `include\scoped_set_reset.h`
- `include\search_stack.h`
- `include\singleton.h`
- `include\status_popup.h`
- `include\streamwrapper.h`
- `include\string_any_map.h`
- `include\string_utils.h`
- `include\stroke_params.h`
- `include\stroke_params_parser.h`
- `include\systemdirsappend.h`
- `include\template_fieldnames.h`
- `include\textentry_tricks.h`
- `include\thread_pool.h`
- `include\title_block.h`
- `include\trace_helpers.h`
- `include\ui_events.h`
- `include\undo_redo_container.h`
- `include\units_provider.h`
- `include\validators.h`
- `include\wildcards_and_files_ext.h`
- `include\wx_filename.h`
- `include\wx_fstream_progress.h`
- `include\wxstream_helper.h`
- `include\xnode.h`
- `include\zoom_defines.h`

### include\api/

- `include\api\api_enums.h`
- `include\api\api_handler.h`
- `include\api\api_handler_common.h`
- `include\api\api_handler_editor.h`
- `include\api\api_plugin.h`
- `include\api\api_plugin_manager.h`
- `include\api\api_server.h`
- `include\api\api_utils.h`
- `include\api\serializable.h`

### include\bitmaps/

- `include\bitmaps\bitmap_info.h`
- `include\bitmaps\bitmap_types.h`
- `include\bitmaps\bitmaps_list.h`

### include\cli/

- `include\cli\exit_codes.h`

### include\core/

- `include\core\mirror.h`
- `include\core\typeinfo.h`

### include\database/

- `include\database\database_cache.h`
- `include\database\database_connection.h`
- `include\database\database_lib_settings.h`

### include\dialogs/

- `include\dialogs\dialog_assign_netclass.h`
- `include\dialogs\dialog_book_reporter.h`
- `include\dialogs\dialog_color_picker.h`
- `include\dialogs\dialog_configure_paths.h`
- `include\dialogs\dialog_edit_library_tables.h`
- `include\dialogs\dialog_embed_files.h`
- `include\dialogs\dialog_global_lib_table_config.h`
- `include\dialogs\dialog_grid_settings.h`
- `include\dialogs\dialog_hotkey_list.h`
- `include\dialogs\dialog_locked_items_query.h`
- `include\dialogs\dialog_migrate_settings.h`
- `include\dialogs\dialog_multi_unit_entry.h`
- `include\dialogs\dialog_page_settings.h`
- `include\dialogs\dialog_paste_special.h`
- `include\dialogs\dialog_plugin_options.h`
- `include\dialogs\dialog_print_generic.h`
- `include\dialogs\dialog_text_entry.h`
- `include\dialogs\dialog_unit_entry.h`
- `include\dialogs\eda_reorderable_list_dialog.h`
- `include\dialogs\eda_view_switcher.h`
- `include\dialogs\html_message_box.h`
- `include\dialogs\panel_color_settings.h`
- `include\dialogs\panel_common_settings.h`
- `include\dialogs\panel_data_collection.h`
- `include\dialogs\panel_gal_display_options.h`
- `include\dialogs\panel_grid_settings.h`
- `include\dialogs\panel_image_editor.h`
- `include\dialogs\panel_mouse_settings.h`
- `include\dialogs\panel_plugin_settings.h`
- `include\dialogs\panel_setup_netclasses.h`
- `include\dialogs\panel_setup_severities.h`

### include\drawing_sheet/

- `include\drawing_sheet\ds_data_item.h`
- `include\drawing_sheet\ds_data_model.h`
- `include\drawing_sheet\ds_draw_item.h`
- `include\drawing_sheet\ds_file_versions.h`
- `include\drawing_sheet\ds_painter.h`
- `include\drawing_sheet\ds_proxy_undo_item.h`
- `include\drawing_sheet\ds_proxy_view_item.h`

### include\font/

- `include\font\font.h`
- `include\font\fontconfig.h`
- `include\font\fontinfo.h`
- `include\font\glyph.h`
- `include\font\kicad_font_name.h`
- `include\font\outline_decomposer.h`
- `include\font\outline_font.h`
- `include\font\stroke_font.h`
- `include\font\text_attributes.h`
- `include\font\version_info.h`

### include\gal/

- `include\gal\color4d.h`
- `include\gal\compositor.h`
- `include\gal\cursors.h`
- `include\gal\definitions.h`
- `include\gal\gal.h`
- `include\gal\gal_display_options.h`
- `include\gal\gal_print.h`
- `include\gal\graphics_abstraction_layer.h`
- `include\gal\hidpi_gl_3D_canvas.h`
- `include\gal\hidpi_gl_canvas.h`
- `include\gal\painter.h`

### include\gal\3d/

- `include\gal\3d\camera.h`

### include\gal\cairo/

- `include\gal\cairo\cairo_compositor.h`
- `include\gal\cairo\cairo_gal.h`
- `include\gal\cairo\cairo_print.h`

### include\gal\opengl/

- `include\gal\opengl\cached_container.h`
- `include\gal\opengl\cached_container_gpu.h`
- `include\gal\opengl\cached_container_ram.h`
- `include\gal\opengl\gl_context_mgr.h`
- `include\gal\opengl\gl_utils.h`
- `include\gal\opengl\gpu_manager.h`
- `include\gal\opengl\kiglew.h`
- `include\gal\opengl\noncached_container.h`
- `include\gal\opengl\opengl_compositor.h`
- `include\gal\opengl\opengl_gal.h`
- `include\gal\opengl\shader.h`
- `include\gal\opengl\utils.h`
- `include\gal\opengl\vertex_common.h`
- `include\gal\opengl\vertex_container.h`
- `include\gal\opengl\vertex_item.h`
- `include\gal\opengl\vertex_manager.h`

### include\http_lib/

- `include\http_lib\http_lib_connection.h`
- `include\http_lib\http_lib_settings.h`

### include\io/

- `include\io\io_base.h`
- `include\io\io_mgr.h`

### include\io\kicad/

- `include\io\kicad\kicad_io_utils.h`

### include\kicad_curl/

- `include\kicad_curl\kicad_curl.h`
- `include\kicad_curl\kicad_curl_easy.h`

### include\libeval/

- `include\libeval\numeric_evaluator.h`

### include\libeval_compiler/

- `include\libeval_compiler\libeval_compiler.h`

### include\plotters/

- `include\plotters\gbr_plotter_aperture_macros.h`
- `include\plotters\gbr_plotter_apertures.h`
- `include\plotters\plotter.h`
- `include\plotters\plotter_dxf.h`
- `include\plotters\plotter_gerber.h`
- `include\plotters\plotter_hpgl.h`
- `include\plotters\plotters_pslike.h`

### include\plugins/

- `include\plugins\kicad_plugin.h`

### include\plugins\3d/

- `include\plugins\3d\3d_plugin.h`

### include\plugins\3dapi/

- `include\plugins\3dapi\c3dmodel.h`
- `include\plugins\3dapi\ifsg_all.h`
- `include\plugins\3dapi\ifsg_api.h`
- `include\plugins\3dapi\ifsg_appearance.h`
- `include\plugins\3dapi\ifsg_colors.h`
- `include\plugins\3dapi\ifsg_coordindex.h`
- `include\plugins\3dapi\ifsg_coords.h`
- `include\plugins\3dapi\ifsg_defs.h`
- `include\plugins\3dapi\ifsg_faceset.h`
- `include\plugins\3dapi\ifsg_index.h`
- `include\plugins\3dapi\ifsg_node.h`
- `include\plugins\3dapi\ifsg_normals.h`
- `include\plugins\3dapi\ifsg_shape.h`
- `include\plugins\3dapi\ifsg_transform.h`
- `include\plugins\3dapi\sg_base.h`
- `include\plugins\3dapi\sg_types.h`
- `include\plugins\3dapi\xv3d_types.h`

### include\preview_items/

- `include\preview_items\anchor_debug.h`
- `include\preview_items\arc_assistant.h`
- `include\preview_items\arc_geom_manager.h`
- `include\preview_items\bezier_assistant.h`
- `include\preview_items\bezier_geom_manager.h`
- `include\preview_items\centreline_rect_item.h`
- `include\preview_items\construction_geom.h`
- `include\preview_items\draw_context.h`
- `include\preview_items\item_drawing_utils.h`
- `include\preview_items\multistep_geom_manager.h`
- `include\preview_items\polygon_geom_manager.h`
- `include\preview_items\polygon_item.h`
- `include\preview_items\preview_utils.h`
- `include\preview_items\ruler_item.h`
- `include\preview_items\selection_area.h`
- `include\preview_items\simple_overlay_item.h`
- `include\preview_items\snap_indicator.h`
- `include\preview_items\two_point_assistant.h`
- `include\preview_items\two_point_geom_manager.h`

### include\project/

- `include\project\board_project_settings.h`
- `include\project\net_settings.h`
- `include\project\project_archiver.h`
- `include\project\project_file.h`
- `include\project\project_local_settings.h`
- `include\project\sch_project_settings.h`

### include\properties/

- `include\properties\color4d_variant.h`
- `include\properties\eda_angle_variant.h`
- `include\properties\pg_cell_renderer.h`
- `include\properties\pg_editors.h`
- `include\properties\pg_properties.h`
- `include\properties\property.h`
- `include\properties\property_mgr.h`
- `include\properties\property_validator.h`
- `include\properties\property_validators.h`
- `include\properties\std_optional_variants.h`

### include\settings/

- `include\settings\app_settings.h`
- `include\settings\aui_settings.h`
- `include\settings\bom_settings.h`
- `include\settings\color_settings.h`
- `include\settings\common_settings.h`
- `include\settings\cvpcb_settings.h`
- `include\settings\environment.h`
- `include\settings\grid_settings.h`
- `include\settings\json_settings.h`
- `include\settings\json_settings_internals.h`
- `include\settings\kicad_settings.h`
- `include\settings\layer_settings_utils.h`
- `include\settings\nested_settings.h`
- `include\settings\parameters.h`
- `include\settings\settings_manager.h`

### include\tool/

- `include\tool\action_manager.h`
- `include\tool\action_menu.h`
- `include\tool\action_toolbar.h`
- `include\tool\actions.h`
- `include\tool\common_control.h`
- `include\tool\common_tools.h`
- `include\tool\conditional_menu.h`
- `include\tool\construction_manager.h`
- `include\tool\coroutine.h`
- `include\tool\edit_constraints.h`
- `include\tool\edit_points.h`
- `include\tool\edit_table_tool_base.h`
- `include\tool\editor_conditions.h`
- `include\tool\embed_tool.h`
- `include\tool\grid_helper.h`
- `include\tool\grid_menu.h`
- `include\tool\picker_tool.h`
- `include\tool\point_editor_behavior.h`
- `include\tool\properties_tool.h`
- `include\tool\selection.h`
- `include\tool\selection_conditions.h`
- `include\tool\selection_tool.h`
- `include\tool\tool_action.h`
- `include\tool\tool_base.h`
- `include\tool\tool_dispatcher.h`
- `include\tool\tool_event.h`
- `include\tool\tool_interactive.h`
- `include\tool\tool_manager.h`
- `include\tool\tool_menu.h`
- `include\tool\tools_holder.h`
- `include\tool\zoom_menu.h`
- `include\tool\zoom_tool.h`

### include\view/

- `include\view\view.h`
- `include\view\view_controls.h`
- `include\view\view_group.h`
- `include\view\view_item.h`
- `include\view\view_overlay.h`
- `include\view\view_rtree.h`
- `include\view\wx_view_controls.h`
- `include\view\zoom_controller.h`

### include\widgets/

- `include\widgets\app_progress_dialog.h`
- `include\widgets\bitmap_button.h`
- `include\widgets\bitmap_toggle.h`
- `include\widgets\busy_indicator.h`
- `include\widgets\button_row_panel.h`
- `include\widgets\color_swatch.h`
- `include\widgets\filedlg_import_non_kicad.h`
- `include\widgets\filedlg_open_embed_file.h`
- `include\widgets\filter_combobox.h`
- `include\widgets\font_choice.h`
- `include\widgets\footprint_choice.h`
- `include\widgets\footprint_diff_widget.h`
- `include\widgets\footprint_preview_widget.h`
- `include\widgets\footprint_select_widget.h`
- `include\widgets\gal_options_panel.h`
- `include\widgets\grid_bitmap_toggle.h`
- `include\widgets\grid_checkbox.h`
- `include\widgets\grid_color_swatch_helpers.h`
- `include\widgets\grid_combobox.h`
- `include\widgets\grid_icon_text_helpers.h`
- `include\widgets\grid_readonly_text_helpers.h`
- `include\widgets\grid_text_button_helpers.h`
- `include\widgets\grid_text_helpers.h`
- `include\widgets\html_window.h`
- `include\widgets\indicator_icon.h`
- `include\widgets\kistatusbar.h`
- `include\widgets\layer_box_selector.h`
- `include\widgets\layer_presentation.h`
- `include\widgets\lib_tree.h`
- `include\widgets\listbox_tricks.h`
- `include\widgets\mathplot.h`
- `include\widgets\msgpanel.h`
- `include\widgets\net_selector.h`
- `include\widgets\number_badge.h`
- `include\widgets\paged_dialog.h`
- `include\widgets\progress_reporter_base.h`
- `include\widgets\report_severity.h`
- `include\widgets\resettable_panel.h`
- `include\widgets\search_pane.h`
- `include\widgets\search_pane_tab.h`
- `include\widgets\split_button.h`
- `include\widgets\std_bitmap_button.h`
- `include\widgets\stepped_slider.h`
- `include\widgets\tab_traversal.h`
- `include\widgets\text_ctrl_eval.h`
- `include\widgets\ui_common.h`
- `include\widgets\unit_binder.h`
- `include\widgets\widget_hotkey_list.h`
- `include\widgets\widget_save_restore.h`
- `include\widgets\wx_aui_art_providers.h`
- `include\widgets\wx_aui_utils.h`
- `include\widgets\wx_busy_indicator.h`
- `include\widgets\wx_collapsible_pane.h`
- `include\widgets\wx_combobox.h`
- `include\widgets\wx_dataviewctrl.h`
- `include\widgets\wx_grid.h`
- `include\widgets\wx_grid_autosizer.h`
- `include\widgets\wx_infobar.h`
- `include\widgets\wx_listbox.h`
- `include\widgets\wx_menubar.h`
- `include\widgets\wx_panel.h`
- `include\widgets\wx_progress_reporters.h`
- `include\widgets\wx_splash.h`
- `include\widgets\wx_treebook.h`

### kicad/

- `kicad\kicad_manager_frame.h`

### libs\core\include\core/

- `libs\core\include\core\arraydim.h`
- `libs\core\include\core\base64.h`
- `libs\core\include\core\ignore.h`
- `libs\core\include\core\json_serializers.h`
- `libs\core\include\core\kicad_algo.h`
- `libs\core\include\core\map_helpers.h`
- `libs\core\include\core\minoptmax.h`
- `libs\core\include\core\multivector.h`
- `libs\core\include\core\observable.h`
- `libs\core\include\core\profile.h`
- `libs\core\include\core\raii.h`
- `libs\core\include\core\spinlock.h`
- `libs\core\include\core\sync_queue.h`
- `libs\core\include\core\type_helpers.h`
- `libs\core\include\core\utf8.h`
- `libs\core\include\core\version_compare.h`
- `libs\core\include\core\wx_stl_compat.h`

### libs\kimath\include/

- `libs\kimath\include\bezier_curves.h`
- `libs\kimath\include\convert_basic_shapes_to_polygon.h`
- `libs\kimath\include\hash.h`
- `libs\kimath\include\hash_128.h`
- `libs\kimath\include\mmh3_hash.h`
- `libs\kimath\include\transform.h`
- `libs\kimath\include\trigo.h`

### libs\kimath\include\geometry/

- `libs\kimath\include\geometry\approximation.h`
- `libs\kimath\include\geometry\circle.h`
- `libs\kimath\include\geometry\convex_hull.h`
- `libs\kimath\include\geometry\corner_operations.h`
- `libs\kimath\include\geometry\corner_strategy.h`
- `libs\kimath\include\geometry\direction45.h`
- `libs\kimath\include\geometry\distribute.h`
- `libs\kimath\include\geometry\eda_angle.h`
- `libs\kimath\include\geometry\ellipse.h`
- `libs\kimath\include\geometry\geometry_utils.h`
- `libs\kimath\include\geometry\half_line.h`
- `libs\kimath\include\geometry\intersection.h`
- `libs\kimath\include\geometry\line.h`
- `libs\kimath\include\geometry\nearest.h`
- `libs\kimath\include\geometry\oval.h`
- `libs\kimath\include\geometry\point_types.h`
- `libs\kimath\include\geometry\polygon_triangulation.h`
- `libs\kimath\include\geometry\roundrect.h`
- `libs\kimath\include\geometry\seg.h`
- `libs\kimath\include\geometry\shape.h`
- `libs\kimath\include\geometry\shape_arc.h`
- `libs\kimath\include\geometry\shape_circle.h`
- `libs\kimath\include\geometry\shape_compound.h`
- `libs\kimath\include\geometry\shape_index.h`
- `libs\kimath\include\geometry\shape_index_list.h`
- `libs\kimath\include\geometry\shape_line_chain.h`
- `libs\kimath\include\geometry\shape_null.h`
- `libs\kimath\include\geometry\shape_poly_set.h`
- `libs\kimath\include\geometry\shape_rect.h`
- `libs\kimath\include\geometry\shape_segment.h`
- `libs\kimath\include\geometry\shape_simple.h`
- `libs\kimath\include\geometry\shape_utils.h`
- `libs\kimath\include\geometry\vector_utils.h`
- `libs\kimath\include\geometry\vertex_set.h`

### libs\kimath\include\math/

- `libs\kimath\include\math\box2.h`
- `libs\kimath\include\math\box2_minmax.h`
- `libs\kimath\include\math\matrix3x3.h`
- `libs\kimath\include\math\util.h`
- `libs\kimath\include\math\vector2d.h`
- `libs\kimath\include\math\vector2wx.h`
- `libs\kimath\include\math\vector3.h`

### libs\kinng\include/

- `libs\kinng\include\kinng.h`

### libs\kiplatform\include\kiplatform/

- `libs\kiplatform\include\kiplatform\app.h`
- `libs\kiplatform\include\kiplatform\drivers.h`
- `libs\kiplatform\include\kiplatform\environment.h`
- `libs\kiplatform\include\kiplatform\io.h`
- `libs\kiplatform\include\kiplatform\policy.h`
- `libs\kiplatform\include\kiplatform\secrets.h`
- `libs\kiplatform\include\kiplatform\ui.h`

### libs\sexpr\include\sexpr/

- `libs\sexpr\include\sexpr\isexprable.h`
- `libs\sexpr\include\sexpr\sexpr.h`
- `libs\sexpr\include\sexpr\sexpr_exception.h`
- `libs\sexpr\include\sexpr\sexpr_parser.h`

### pcbnew/

- `pcbnew\action_plugin.h`
- `pcbnew\array_pad_number_provider.h`
- `pcbnew\board.h`
- `pcbnew\board_commit.h`
- `pcbnew\board_connected_item.h`
- `pcbnew\board_item_container.h`
- `pcbnew\cleanup_item.h`
- `pcbnew\collectors.h`
- `pcbnew\component_class_manager.h`
- `pcbnew\convert_shape_list_to_polygon.h`
- `pcbnew\fix_board_shape.h`
- `pcbnew\footprint.h`
- `pcbnew\footprint_chooser_frame.h`
- `pcbnew\footprint_edit_frame.h`
- `pcbnew\footprint_info_impl.h`
- `pcbnew\footprint_preview_panel.h`
- `pcbnew\footprint_tree_pane.h`
- `pcbnew\footprint_viewer_frame.h`
- `pcbnew\footprint_wizard.h`
- `pcbnew\footprint_wizard_frame.h`
- `pcbnew\fp_tree_model_adapter.h`
- `pcbnew\fp_tree_synchronizing_adapter.h`
- `pcbnew\generate_footprint_info.h`
- `pcbnew\generators_mgr.h`
- `pcbnew\graphics_cleaner.h`
- `pcbnew\grid_layer_box_helpers.h`
- `pcbnew\invoke_pcb_dialog.h`
- `pcbnew\kicad_clipboard.h`
- `pcbnew\layer_pairs.h`
- `pcbnew\netinfo.h`
- `pcbnew\pad.h`
- `pcbnew\pad_utils.h`
- `pcbnew\padstack.h`
- `pcbnew\pcb_base_edit_frame.h`
- `pcbnew\pcb_dimension.h`
- `pcbnew\pcb_draw_panel_gal.h`
- `pcbnew\pcb_edit_frame.h`
- `pcbnew\pcb_field.h`
- `pcbnew\pcb_fields_grid_table.h`
- `pcbnew\pcb_generator.h`
- `pcbnew\pcb_group.h`
- `pcbnew\pcb_item_containers.h`
- `pcbnew\pcb_layer_box_selector.h`
- `pcbnew\pcb_layer_presentation.h`
- `pcbnew\pcb_marker.h`
- `pcbnew\pcb_origin_transforms.h`
- `pcbnew\pcb_painter.h`
- `pcbnew\pcb_plot_params.h`
- `pcbnew\pcb_plot_params_parser.h`
- `pcbnew\pcb_plotter.h`
- `pcbnew\pcb_reference_image.h`
- `pcbnew\pcb_shape.h`
- `pcbnew\pcb_table.h`
- `pcbnew\pcb_tablecell.h`
- `pcbnew\pcb_target.h`
- `pcbnew\pcb_text.h`
- `pcbnew\pcb_textbox.h`
- `pcbnew\pcb_track.h`
- `pcbnew\pcb_view.h`
- `pcbnew\pcbexpr_evaluator.h`
- `pcbnew\pcbnew_id.h`
- `pcbnew\pcbnew_jobs_handler.h`
- `pcbnew\pcbnew_printout.h`
- `pcbnew\pcbnew_settings.h`
- `pcbnew\pcbplot.h`
- `pcbnew\plotcontroller.h`
- `pcbnew\plotprint_opts.h`
- `pcbnew\tracks_cleaner.h`
- `pcbnew\zone.h`
- `pcbnew\zone_filler.h`
- `pcbnew\zone_settings.h`
- `pcbnew\zones.h`

### pcbnew\api/

- `pcbnew\api\api_handler_pcb.h`
- `pcbnew\api\api_pcb_utils.h`

### pcbnew\autorouter/

- `pcbnew\autorouter\ar_autoplacer.h`
- `pcbnew\autorouter\ar_matrix.h`
- `pcbnew\autorouter\autoplace_tool.h`
- `pcbnew\autorouter\spread_footprints.h`

### pcbnew\board_stackup_manager/

- `pcbnew\board_stackup_manager\board_stackup.h`
- `pcbnew\board_stackup_manager\board_stackup_reporter.h`
- `pcbnew\board_stackup_manager\dialog_dielectric_list_manager.h`
- `pcbnew\board_stackup_manager\dialog_dielectric_list_manager_base.h`
- `pcbnew\board_stackup_manager\dielectric_material.h`
- `pcbnew\board_stackup_manager\panel_board_finish.h`
- `pcbnew\board_stackup_manager\panel_board_finish_base.h`
- `pcbnew\board_stackup_manager\panel_board_stackup.h`
- `pcbnew\board_stackup_manager\panel_board_stackup_base.h`
- `pcbnew\board_stackup_manager\stackup_predefined_prms.h`

### pcbnew\connectivity/

- `pcbnew\connectivity\connectivity_algo.h`
- `pcbnew\connectivity\connectivity_data.h`
- `pcbnew\connectivity\connectivity_items.h`
- `pcbnew\connectivity\connectivity_rtree.h`
- `pcbnew\connectivity\from_to_cache.h`
- `pcbnew\connectivity\topo_match.h`

### pcbnew\dialogs/

- `pcbnew\dialogs\dialog_board_reannotate.h`
- `pcbnew\dialogs\dialog_board_reannotate_base.h`
- `pcbnew\dialogs\dialog_board_setup.h`
- `pcbnew\dialogs\dialog_board_statistics.h`
- `pcbnew\dialogs\dialog_board_statistics_base.h`
- `pcbnew\dialogs\dialog_cleanup_graphics.h`
- `pcbnew\dialogs\dialog_cleanup_graphics_base.h`
- `pcbnew\dialogs\dialog_cleanup_tracks_and_vias.h`
- `pcbnew\dialogs\dialog_cleanup_tracks_and_vias_base.h`
- `pcbnew\dialogs\dialog_copper_zones_base.h`
- `pcbnew\dialogs\dialog_create_array.h`
- `pcbnew\dialogs\dialog_create_array_base.h`
- `pcbnew\dialogs\dialog_dimension_properties.h`
- `pcbnew\dialogs\dialog_dimension_properties_base.h`
- `pcbnew\dialogs\dialog_drc.h`
- `pcbnew\dialogs\dialog_drc_base.h`
- `pcbnew\dialogs\dialog_drc_job_config.h`
- `pcbnew\dialogs\dialog_enum_pads.h`
- `pcbnew\dialogs\dialog_enum_pads_base.h`
- `pcbnew\dialogs\dialog_exchange_footprints.h`
- `pcbnew\dialogs\dialog_exchange_footprints_base.h`
- `pcbnew\dialogs\dialog_export_2581.h`
- `pcbnew\dialogs\dialog_export_2581_base.h`
- `pcbnew\dialogs\dialog_export_idf.h`
- `pcbnew\dialogs\dialog_export_idf_base.h`
- `pcbnew\dialogs\dialog_export_odbpp.h`
- `pcbnew\dialogs\dialog_export_odbpp_base.h`
- `pcbnew\dialogs\dialog_export_step.h`
- `pcbnew\dialogs\dialog_export_step_base.h`
- `pcbnew\dialogs\dialog_export_step_process.h`
- `pcbnew\dialogs\dialog_export_step_process_base.h`
- `pcbnew\dialogs\dialog_export_vrml.h`
- `pcbnew\dialogs\dialog_export_vrml_base.h`
- `pcbnew\dialogs\dialog_filter_selection.h`
- `pcbnew\dialogs\dialog_filter_selection_base.h`
- `pcbnew\dialogs\dialog_find.h`
- `pcbnew\dialogs\dialog_find_base.h`
- `pcbnew\dialogs\dialog_footprint_associations.h`
- `pcbnew\dialogs\dialog_footprint_associations_base.h`
- `pcbnew\dialogs\dialog_footprint_checker.h`
- `pcbnew\dialogs\dialog_footprint_checker_base.h`
- `pcbnew\dialogs\dialog_footprint_properties.h`
- `pcbnew\dialogs\dialog_footprint_properties_base.h`
- `pcbnew\dialogs\dialog_footprint_properties_fp_editor.h`
- `pcbnew\dialogs\dialog_footprint_properties_fp_editor_base.h`
- `pcbnew\dialogs\dialog_footprint_wizard_list.h`
- `pcbnew\dialogs\dialog_footprint_wizard_list_base.h`
- `pcbnew\dialogs\dialog_gen_footprint_position.h`
- `pcbnew\dialogs\dialog_gen_footprint_position_file_base.h`
- `pcbnew\dialogs\dialog_gencad_export_options.h`
- `pcbnew\dialogs\dialog_gendrill.h`
- `pcbnew\dialogs\dialog_gendrill_base.h`
- `pcbnew\dialogs\dialog_generators.h`
- `pcbnew\dialogs\dialog_generators_base.h`
- `pcbnew\dialogs\dialog_get_footprint_by_name.h`
- `pcbnew\dialogs\dialog_get_footprint_by_name_base.h`
- `pcbnew\dialogs\dialog_global_deletion.h`
- `pcbnew\dialogs\dialog_global_deletion_base.h`
- `pcbnew\dialogs\dialog_global_edit_teardrops_base.h`
- `pcbnew\dialogs\dialog_global_edit_text_and_graphics_base.h`
- `pcbnew\dialogs\dialog_global_edit_tracks_and_vias.h`
- `pcbnew\dialogs\dialog_global_edit_tracks_and_vias_base.h`
- `pcbnew\dialogs\dialog_global_fp_lib_table_config.h`
- `pcbnew\dialogs\dialog_group_properties.h`
- `pcbnew\dialogs\dialog_group_properties_base.h`
- `pcbnew\dialogs\dialog_import_netlist.h`
- `pcbnew\dialogs\dialog_import_netlist_base.h`
- `pcbnew\dialogs\dialog_import_settings.h`
- `pcbnew\dialogs\dialog_import_settings_base.h`
- `pcbnew\dialogs\dialog_imported_layers_base.h`
- `pcbnew\dialogs\dialog_layer_selection_base.h`
- `pcbnew\dialogs\dialog_map_layers.h`
- `pcbnew\dialogs\dialog_move_exact.h`
- `pcbnew\dialogs\dialog_move_exact_base.h`
- `pcbnew\dialogs\dialog_multichannel_generate_rule_areas.h`
- `pcbnew\dialogs\dialog_multichannel_generate_rule_areas_base.h`
- `pcbnew\dialogs\dialog_multichannel_repeat_layout.h`
- `pcbnew\dialogs\dialog_multichannel_repeat_layout_base.h`
- `pcbnew\dialogs\dialog_non_copper_zones_properties_base.h`
- `pcbnew\dialogs\dialog_outset_items.h`
- `pcbnew\dialogs\dialog_outset_items_base.h`
- `pcbnew\dialogs\dialog_pad_properties.h`
- `pcbnew\dialogs\dialog_pad_properties_base.h`
- `pcbnew\dialogs\dialog_plot.h`
- `pcbnew\dialogs\dialog_plot_base.h`
- `pcbnew\dialogs\dialog_pns_diff_pair_dimensions.h`
- `pcbnew\dialogs\dialog_pns_diff_pair_dimensions_base.h`
- `pcbnew\dialogs\dialog_pns_settings.h`
- `pcbnew\dialogs\dialog_pns_settings_base.h`
- `pcbnew\dialogs\dialog_position_relative.h`
- `pcbnew\dialogs\dialog_position_relative_base.h`
- `pcbnew\dialogs\dialog_push_pad_properties.h`
- `pcbnew\dialogs\dialog_push_pad_properties_base.h`
- `pcbnew\dialogs\dialog_reference_image_properties.h`
- `pcbnew\dialogs\dialog_reference_image_properties_base.h`
- `pcbnew\dialogs\dialog_render_job.h`
- `pcbnew\dialogs\dialog_render_job_base.h`
- `pcbnew\dialogs\dialog_rule_area_properties_base.h`
- `pcbnew\dialogs\dialog_set_offset.h`
- `pcbnew\dialogs\dialog_set_offset_base.h`
- `pcbnew\dialogs\dialog_shape_properties_base.h`
- `pcbnew\dialogs\dialog_swap_layers.h`
- `pcbnew\dialogs\dialog_swap_layers_base.h`
- `pcbnew\dialogs\dialog_table_properties.h`
- `pcbnew\dialogs\dialog_table_properties_base.h`
- `pcbnew\dialogs\dialog_tablecell_properties.h`
- `pcbnew\dialogs\dialog_tablecell_properties_base.h`
- `pcbnew\dialogs\dialog_target_properties_base.h`
- `pcbnew\dialogs\dialog_text_properties.h`
- `pcbnew\dialogs\dialog_text_properties_base.h`
- `pcbnew\dialogs\dialog_textbox_properties.h`
- `pcbnew\dialogs\dialog_textbox_properties_base.h`
- `pcbnew\dialogs\dialog_track_via_properties.h`
- `pcbnew\dialogs\dialog_track_via_properties_base.h`
- `pcbnew\dialogs\dialog_track_via_size.h`
- `pcbnew\dialogs\dialog_track_via_size_base.h`
- `pcbnew\dialogs\dialog_tuning_pattern_properties.h`
- `pcbnew\dialogs\dialog_tuning_pattern_properties_base.h`
- `pcbnew\dialogs\dialog_unused_pad_layers.h`
- `pcbnew\dialogs\dialog_unused_pad_layers_base.h`
- `pcbnew\dialogs\dialog_update_pcb.h`
- `pcbnew\dialogs\dialog_update_pcb_base.h`
- `pcbnew\dialogs\panel_edit_options.h`
- `pcbnew\dialogs\panel_edit_options_base.h`
- `pcbnew\dialogs\panel_fp_editor_color_settings.h`
- `pcbnew\dialogs\panel_fp_editor_field_defaults.h`
- `pcbnew\dialogs\panel_fp_editor_field_defaults_base.h`
- `pcbnew\dialogs\panel_fp_editor_graphics_defaults.h`
- `pcbnew\dialogs\panel_fp_editor_graphics_defaults_base.h`
- `pcbnew\dialogs\panel_fp_lib_table.h`
- `pcbnew\dialogs\panel_fp_lib_table_base.h`
- `pcbnew\dialogs\panel_fp_properties_3d_model.h`
- `pcbnew\dialogs\panel_fp_properties_3d_model_base.h`
- `pcbnew\dialogs\panel_pcb_display_options.h`
- `pcbnew\dialogs\panel_pcb_display_options_base.h`
- `pcbnew\dialogs\panel_pcbnew_action_plugins.h`
- `pcbnew\dialogs\panel_pcbnew_action_plugins_base.h`
- `pcbnew\dialogs\panel_pcbnew_color_settings.h`
- `pcbnew\dialogs\panel_pcbnew_display_origin.h`
- `pcbnew\dialogs\panel_pcbnew_display_origin_base.h`
- `pcbnew\dialogs\panel_rule_area_properties_keepout_base.h`
- `pcbnew\dialogs\panel_rule_area_properties_placement_base.h`
- `pcbnew\dialogs\panel_setup_constraints.h`
- `pcbnew\dialogs\panel_setup_constraints_base.h`
- `pcbnew\dialogs\panel_setup_dimensions.h`
- `pcbnew\dialogs\panel_setup_dimensions_base.h`
- `pcbnew\dialogs\panel_setup_formatting.h`
- `pcbnew\dialogs\panel_setup_formatting_base.h`
- `pcbnew\dialogs\panel_setup_layers.h`
- `pcbnew\dialogs\panel_setup_layers_base.h`
- `pcbnew\dialogs\panel_setup_mask_and_paste.h`
- `pcbnew\dialogs\panel_setup_mask_and_paste_base.h`
- `pcbnew\dialogs\panel_setup_rules.h`
- `pcbnew\dialogs\panel_setup_rules_base.h`
- `pcbnew\dialogs\panel_setup_rules_help_10documentation.h`
- `pcbnew\dialogs\panel_setup_rules_help_1clauses.h`
- `pcbnew\dialogs\panel_setup_rules_help_2constraints.h`
- `pcbnew\dialogs\panel_setup_rules_help_3items.h`
- `pcbnew\dialogs\panel_setup_rules_help_4severity_names.h`
- `pcbnew\dialogs\panel_setup_rules_help_5examples.h`
- `pcbnew\dialogs\panel_setup_rules_help_6notes.h`
- `pcbnew\dialogs\panel_setup_rules_help_7properties.h`
- `pcbnew\dialogs\panel_setup_rules_help_8expression_functions.h`
- `pcbnew\dialogs\panel_setup_rules_help_9more_examples.h`
- `pcbnew\dialogs\panel_setup_teardrops.h`
- `pcbnew\dialogs\panel_setup_teardrops_base.h`
- `pcbnew\dialogs\panel_setup_text_and_graphics.h`
- `pcbnew\dialogs\panel_setup_text_and_graphics_base.h`
- `pcbnew\dialogs\panel_setup_tracks_and_vias.h`
- `pcbnew\dialogs\panel_setup_tracks_and_vias_base.h`
- `pcbnew\dialogs\panel_setup_tuning_patterns.h`
- `pcbnew\dialogs\panel_setup_tuning_patterns_base.h`

### pcbnew\drc/

- `pcbnew\drc\drc_cache_generator.h`
- `pcbnew\drc\drc_creepage_utils.h`
- `pcbnew\drc\drc_engine.h`
- `pcbnew\drc\drc_interactive_courtyard_clearance.h`
- `pcbnew\drc\drc_item.h`
- `pcbnew\drc\drc_report.h`
- `pcbnew\drc\drc_rtree.h`
- `pcbnew\drc\drc_rule.h`
- `pcbnew\drc\drc_rule_condition.h`
- `pcbnew\drc\drc_rule_parser.h`
- `pcbnew\drc\drc_test_provider.h`
- `pcbnew\drc\drc_test_provider_clearance_base.h`

### pcbnew\exporters/

- `pcbnew\exporters\board_exporter_base.h`
- `pcbnew\exporters\export_d356.h`
- `pcbnew\exporters\export_gencad_writer.h`
- `pcbnew\exporters\export_svg.h`
- `pcbnew\exporters\export_vrml.h`
- `pcbnew\exporters\exporter_vrml.h`
- `pcbnew\exporters\gendrill_Excellon_writer.h`
- `pcbnew\exporters\gendrill_file_writer_base.h`
- `pcbnew\exporters\gendrill_gerber_writer.h`
- `pcbnew\exporters\gerber_jobfile_writer.h`
- `pcbnew\exporters\gerber_placefile_writer.h`
- `pcbnew\exporters\place_file_exporter.h`

### pcbnew\exporters\step/

- `pcbnew\exporters\step\KI_XCAFDoc_AssemblyGraph.hxx`
- `pcbnew\exporters\step\exporter_step.h`
- `pcbnew\exporters\step\step_pcb_model.h`

### pcbnew\import_gfx/

- `pcbnew\import_gfx\dialog_import_graphics.h`
- `pcbnew\import_gfx\dialog_import_graphics_base.h`
- `pcbnew\import_gfx\graphics_importer_pcbnew.h`

### pcbnew\microwave/

- `pcbnew\microwave\microwave_tool.h`

### pcbnew\navlib/

- `pcbnew\navlib\nl_pcbnew_plugin.h`
- `pcbnew\navlib\nl_pcbnew_plugin_impl.h`

### pcbnew\netlist_reader/

- `pcbnew\netlist_reader\board_netlist_updater.h`
- `pcbnew\netlist_reader\kicad_netlist_parser.h`
- `pcbnew\netlist_reader\netlist_reader.h`
- `pcbnew\netlist_reader\pcb_netlist.h`

### pcbnew\pcb_io/

- `pcbnew\pcb_io\pcb_io.h`
- `pcbnew\pcb_io\pcb_io_mgr.h`

### pcbnew\pcb_io\altium/

- `pcbnew\pcb_io\altium\altium_parser_pcb.h`
- `pcbnew\pcb_io\altium\altium_pcb.h`
- `pcbnew\pcb_io\altium\altium_pcb_compound_file.h`
- `pcbnew\pcb_io\altium\pcb_io_altium_circuit_maker.h`
- `pcbnew\pcb_io\altium\pcb_io_altium_circuit_studio.h`
- `pcbnew\pcb_io\altium\pcb_io_altium_designer.h`
- `pcbnew\pcb_io\altium\pcb_io_solidworks.h`

### pcbnew\pcb_io\cadstar/

- `pcbnew\pcb_io\cadstar\cadstar_pcb_archive_loader.h`
- `pcbnew\pcb_io\cadstar\cadstar_pcb_archive_parser.h`
- `pcbnew\pcb_io\cadstar\pcb_io_cadstar_archive.h`

### pcbnew\pcb_io\common/

- `pcbnew\pcb_io\common\plugin_common_layer_mapping.h`

### pcbnew\pcb_io\eagle/

- `pcbnew\pcb_io\eagle\pcb_io_eagle.h`

### pcbnew\pcb_io\easyeda/

- `pcbnew\pcb_io\easyeda\pcb_io_easyeda_parser.h`
- `pcbnew\pcb_io\easyeda\pcb_io_easyeda_plugin.h`

### pcbnew\pcb_io\easyedapro/

- `pcbnew\pcb_io\easyedapro\pcb_io_easyedapro.h`
- `pcbnew\pcb_io\easyedapro\pcb_io_easyedapro_parser.h`

### pcbnew\pcb_io\fabmaster/

- `pcbnew\pcb_io\fabmaster\import_fabmaster.h`
- `pcbnew\pcb_io\fabmaster\pcb_io_fabmaster.h`

### pcbnew\pcb_io\geda/

- `pcbnew\pcb_io\geda\pcb_io_geda.h`

### pcbnew\pcb_io\ipc2581/

- `pcbnew\pcb_io\ipc2581\pcb_io_ipc2581.h`

### pcbnew\pcb_io\kicad_legacy/

- `pcbnew\pcb_io\kicad_legacy\pcb_io_kicad_legacy.h`

### pcbnew\pcb_io\kicad_sexpr/

- `pcbnew\pcb_io\kicad_sexpr\pcb_io_kicad_sexpr.h`
- `pcbnew\pcb_io\kicad_sexpr\pcb_io_kicad_sexpr_parser.h`

### pcbnew\pcb_io\odbpp/

- `pcbnew\pcb_io\odbpp\odb_attribute.h`
- `pcbnew\pcb_io\odbpp\odb_component.h`
- `pcbnew\pcb_io\odbpp\odb_defines.h`
- `pcbnew\pcb_io\odbpp\odb_eda_data.h`
- `pcbnew\pcb_io\odbpp\odb_entity.h`
- `pcbnew\pcb_io\odbpp\odb_feature.h`
- `pcbnew\pcb_io\odbpp\odb_netlist.h`
- `pcbnew\pcb_io\odbpp\odb_util.h`
- `pcbnew\pcb_io\odbpp\pcb_io_odbpp.h`

### pcbnew\pcb_io\pcad/

- `pcbnew\pcb_io\pcad\pcad2kicad_common.h`
- `pcbnew\pcb_io\pcad\pcad_arc.h`
- `pcbnew\pcb_io\pcad\pcad_callbacks.h`
- `pcbnew\pcb_io\pcad\pcad_copper_pour.h`
- `pcbnew\pcb_io\pcad\pcad_cutout.h`
- `pcbnew\pcb_io\pcad\pcad_footprint.h`
- `pcbnew\pcb_io\pcad\pcad_item_types.h`
- `pcbnew\pcb_io\pcad\pcad_keepout.h`
- `pcbnew\pcb_io\pcad\pcad_line.h`
- `pcbnew\pcb_io\pcad\pcad_nets.h`
- `pcbnew\pcb_io\pcad\pcad_pad.h`
- `pcbnew\pcb_io\pcad\pcad_pad_shape.h`
- `pcbnew\pcb_io\pcad\pcad_pcb.h`
- `pcbnew\pcb_io\pcad\pcad_pcb_component.h`
- `pcbnew\pcb_io\pcad\pcad_plane.h`
- `pcbnew\pcb_io\pcad\pcad_polygon.h`
- `pcbnew\pcb_io\pcad\pcad_text.h`
- `pcbnew\pcb_io\pcad\pcad_via.h`
- `pcbnew\pcb_io\pcad\pcad_via_shape.h`
- `pcbnew\pcb_io\pcad\pcb_io_pcad.h`
- `pcbnew\pcb_io\pcad\s_expr_loader.h`

### pcbnew\python\scripting/

- `pcbnew\python\scripting\pcb_scripting_tool.h`
- `pcbnew\python\scripting\pcbnew_action_plugins.h`
- `pcbnew\python\scripting\pcbnew_scripting.h`
- `pcbnew\python\scripting\pcbnew_scripting_helpers.h`

### pcbnew\ratsnest/

- `pcbnew\ratsnest\ratsnest_data.h`
- `pcbnew\ratsnest\ratsnest_view_item.h`

### pcbnew\router/

- `pcbnew\router\pns_algo_base.h`
- `pcbnew\router\pns_arc.h`
- `pcbnew\router\pns_component_dragger.h`
- `pcbnew\router\pns_debug_decorator.h`
- `pcbnew\router\pns_diff_pair.h`
- `pcbnew\router\pns_diff_pair_placer.h`
- `pcbnew\router\pns_dp_meander_placer.h`
- `pcbnew\router\pns_drag_algo.h`
- `pcbnew\router\pns_dragger.h`
- `pcbnew\router\pns_hole.h`
- `pcbnew\router\pns_index.h`
- `pcbnew\router\pns_item.h`
- `pcbnew\router\pns_itemset.h`
- `pcbnew\router\pns_joint.h`
- `pcbnew\router\pns_kicad_iface.h`
- `pcbnew\router\pns_layerset.h`
- `pcbnew\router\pns_line.h`
- `pcbnew\router\pns_line_placer.h`
- `pcbnew\router\pns_link_holder.h`
- `pcbnew\router\pns_linked_item.h`
- `pcbnew\router\pns_logger.h`
- `pcbnew\router\pns_meander.h`
- `pcbnew\router\pns_meander_placer.h`
- `pcbnew\router\pns_meander_placer_base.h`
- `pcbnew\router\pns_meander_skew_placer.h`
- `pcbnew\router\pns_mouse_trail_tracer.h`
- `pcbnew\router\pns_multi_dragger.h`
- `pcbnew\router\pns_node.h`
- `pcbnew\router\pns_optimizer.h`
- `pcbnew\router\pns_placement_algo.h`
- `pcbnew\router\pns_router.h`
- `pcbnew\router\pns_routing_settings.h`
- `pcbnew\router\pns_segment.h`
- `pcbnew\router\pns_shove.h`
- `pcbnew\router\pns_sizes_settings.h`
- `pcbnew\router\pns_solid.h`
- `pcbnew\router\pns_tool_base.h`
- `pcbnew\router\pns_topology.h`
- `pcbnew\router\pns_utils.h`
- `pcbnew\router\pns_via.h`
- `pcbnew\router\pns_walkaround.h`
- `pcbnew\router\range.h`
- `pcbnew\router\ranged_num.h`
- `pcbnew\router\router_preview_item.h`
- `pcbnew\router\router_status_view_item.h`
- `pcbnew\router\router_tool.h`
- `pcbnew\router\time_limit.h`

### pcbnew\specctra_import_export/

- `pcbnew\specctra_import_export\specctra.h`

### pcbnew\teardrop/

- `pcbnew\teardrop\teardrop.h`
- `pcbnew\teardrop\teardrop_parameters.h`
- `pcbnew\teardrop\teardrop_types.h`

### pcbnew\tools/

- `pcbnew\tools\align_distribute_tool.h`
- `pcbnew\tools\array_tool.h`
- `pcbnew\tools\board_editor_control.h`
- `pcbnew\tools\board_inspection_tool.h`
- `pcbnew\tools\board_reannotate_tool.h`
- `pcbnew\tools\convert_tool.h`
- `pcbnew\tools\drawing_tool.h`
- `pcbnew\tools\drc_tool.h`
- `pcbnew\tools\edit_tool.h`
- `pcbnew\tools\footprint_chooser_selection_tool.h`
- `pcbnew\tools\footprint_editor_control.h`
- `pcbnew\tools\generator_tool.h`
- `pcbnew\tools\generator_tool_pns_proxy.h`
- `pcbnew\tools\global_edit_tool.h`
- `pcbnew\tools\group_tool.h`
- `pcbnew\tools\item_modification_routine.h`
- `pcbnew\tools\multichannel_tool.h`
- `pcbnew\tools\pad_tool.h`
- `pcbnew\tools\pcb_actions.h`
- `pcbnew\tools\pcb_control.h`
- `pcbnew\tools\pcb_edit_table_tool.h`
- `pcbnew\tools\pcb_editor_conditions.h`
- `pcbnew\tools\pcb_grid_helper.h`
- `pcbnew\tools\pcb_picker_tool.h`
- `pcbnew\tools\pcb_point_editor.h`
- `pcbnew\tools\pcb_selection.h`
- `pcbnew\tools\pcb_selection_conditions.h`
- `pcbnew\tools\pcb_selection_tool.h`
- `pcbnew\tools\pcb_tool_base.h`
- `pcbnew\tools\pcb_tool_utils.h`
- `pcbnew\tools\pcb_viewer_tools.h`
- `pcbnew\tools\position_relative_tool.h`
- `pcbnew\tools\tool_event_utils.h`
- `pcbnew\tools\zone_create_helper.h`
- `pcbnew\tools\zone_filler_tool.h`

### pcbnew\widgets/

- `pcbnew\widgets\appearance_controls.h`
- `pcbnew\widgets\appearance_controls_base.h`
- `pcbnew\widgets\filedlg_hook_save_project.h`
- `pcbnew\widgets\legacyfiledlg_netlist_options.h`
- `pcbnew\widgets\net_inspector_panel.h`
- `pcbnew\widgets\panel_footprint_chooser.h`
- `pcbnew\widgets\panel_selection_filter.h`
- `pcbnew\widgets\panel_selection_filter_base.h`
- `pcbnew\widgets\pcb_net_inspector_panel.h`
- `pcbnew\widgets\pcb_net_inspector_panel_data_model.h`
- `pcbnew\widgets\pcb_properties_panel.h`
- `pcbnew\widgets\pcb_search_pane.h`
- `pcbnew\widgets\search_handlers.h`

### pcbnew\zone_manager/

- `pcbnew\zone_manager\board_edges_bounding_item.h`
- `pcbnew\zone_manager\dialog_zone_manager.h`
- `pcbnew\zone_manager\dialog_zone_manager_base.h`
- `pcbnew\zone_manager\managed_zone.h`
- `pcbnew\zone_manager\model_zones_overview_table.h`
- `pcbnew\zone_manager\pane_zone_viewer.h`
- `pcbnew\zone_manager\panel_zone_gal.h`
- `pcbnew\zone_manager\panel_zone_properties.h`
- `pcbnew\zone_manager\panel_zone_properties_base.h`
- `pcbnew\zone_manager\zone_management_base.h`
- `pcbnew\zone_manager\zone_manager_preference.h`
- `pcbnew\zone_manager\zone_painter.h`
- `pcbnew\zone_manager\zone_selection_change_notifier.h`
- `pcbnew\zone_manager\zones_container.h`

### plugins\ldr/

- `plugins\ldr\pluginldr.h`

### plugins\ldr\3d/

- `plugins\ldr\3d\pluginldr3D.h`

### scripting/

- `scripting\python_manager.h`
- `scripting\python_scripting.h`

### thirdparty\3dxware_sdk\inc/

- `thirdparty\3dxware_sdk\inc\siappcmd_types.h`

### thirdparty\3dxware_sdk\inc\SpaceMouse/

- `thirdparty\3dxware_sdk\inc\SpaceMouse\CActionNode.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CCategory.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CCommand.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CCommandSet.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CCommandTreeNode.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CCookieCollection.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CHitTest.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CImage.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CNavigation3D.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CNavlibImpl.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\CNavlibInterface.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IAccessors.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IEvents.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IHit.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IModel.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\INavlib.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IPivot.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\ISpace3D.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IState.hpp`
- `thirdparty\3dxware_sdk\inc\SpaceMouse\IView.hpp`

### thirdparty\3dxware_sdk\inc\navlib/

- `thirdparty\3dxware_sdk\inc\navlib\navlib.h`
- `thirdparty\3dxware_sdk\inc\navlib\navlib_defines.h`
- `thirdparty\3dxware_sdk\inc\navlib\navlib_error.h`
- `thirdparty\3dxware_sdk\inc\navlib\navlib_ostream.h`
- `thirdparty\3dxware_sdk\inc\navlib\navlib_templates.h`
- `thirdparty\3dxware_sdk\inc\navlib\navlib_types.h`

### thirdparty\clipper2\Clipper2Lib\include\clipper2/

- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.core.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.engine.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.minkowski.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.offset.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.rectclip.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.version.h`

### thirdparty\compoundfilereader/

- `thirdparty\compoundfilereader\compoundfilereader.h`
- `thirdparty\compoundfilereader\utf.h`

### thirdparty\delaunator/

- `thirdparty\delaunator\delaunator.hpp`

### thirdparty\dxflib_qcad/

- `thirdparty\dxflib_qcad\dl_attributes.h`
- `thirdparty\dxflib_qcad\dl_codes.h`
- `thirdparty\dxflib_qcad\dl_creationadapter.h`
- `thirdparty\dxflib_qcad\dl_creationinterface.h`
- `thirdparty\dxflib_qcad\dl_dxf.h`
- `thirdparty\dxflib_qcad\dl_entities.h`
- `thirdparty\dxflib_qcad\dl_exception.h`
- `thirdparty\dxflib_qcad\dl_extrusion.h`
- `thirdparty\dxflib_qcad\dl_global.h`
- `thirdparty\dxflib_qcad\dl_writer.h`
- `thirdparty\dxflib_qcad\dl_writer_ascii.h`

### thirdparty\dynamic_bitset/

- `thirdparty\dynamic_bitset\dynamic_bitset.h`

### thirdparty\expected\include\tl/

- `thirdparty\expected\include\tl\expected.hpp`

### thirdparty\fmt\include\fmt/

- `thirdparty\fmt\include\fmt\base.h`
- `thirdparty\fmt\include\fmt\chrono.h`
- `thirdparty\fmt\include\fmt\core.h`
- `thirdparty\fmt\include\fmt\format-inl.h`
- `thirdparty\fmt\include\fmt\format.h`
- `thirdparty\fmt\include\fmt\ostream.h`

### thirdparty\gzip-hpp/

- `thirdparty\gzip-hpp\decompress.hpp`

### thirdparty\json_schema_validator/

- `thirdparty\json_schema_validator\json-patch.hpp`

### thirdparty\json_schema_validator\nlohmann/

- `thirdparty\json_schema_validator\nlohmann\json-schema.hpp`

### thirdparty\libcontext/

- `thirdparty\libcontext\libcontext.h`

### thirdparty\libpopcnt/

- `thirdparty\libpopcnt\libpopcnt.h`

### thirdparty\magic_enum\magic_enum/

- `thirdparty\magic_enum\magic_enum\magic_enum.hpp`

### thirdparty\markdown2html\html_formatter/

- `thirdparty\markdown2html\html_formatter\houdini.h`
- `thirdparty\markdown2html\html_formatter\html.h`

### thirdparty\markdown2html\md_parser/

- `thirdparty\markdown2html\md_parser\autolink.h`
- `thirdparty\markdown2html\md_parser\buffer.h`
- `thirdparty\markdown2html\md_parser\html_blocks.h`
- `thirdparty\markdown2html\md_parser\markdown.h`
- `thirdparty\markdown2html\md_parser\stack.h`

### thirdparty\nanodbc\nanodbc/

- `thirdparty\nanodbc\nanodbc\nanodbc.h`

### thirdparty\nanosvg/

- `thirdparty\nanosvg\nanosvg.h`

### thirdparty\nlohmann_json\nlohmann/

- `thirdparty\nlohmann_json\nlohmann\json.hpp`
- `thirdparty\nlohmann_json\nlohmann\json_fwd.hpp`

### thirdparty\nlohmann_json\nlohmann\detail/

- `thirdparty\nlohmann_json\nlohmann\detail\abi_macros.hpp`

### thirdparty\other_math/

- `thirdparty\other_math\math_for_graphics.h`

### thirdparty\pegtl/

- `thirdparty\pegtl\pegtl.hpp`

### thirdparty\pegtl\pegtl/

- `thirdparty\pegtl\pegtl\apply_mode.hpp`
- `thirdparty\pegtl\pegtl\argv_input.hpp`
- `thirdparty\pegtl\pegtl\ascii.hpp`
- `thirdparty\pegtl\pegtl\buffer_input.hpp`
- `thirdparty\pegtl\pegtl\change_action.hpp`
- `thirdparty\pegtl\pegtl\change_action_and_state.hpp`
- `thirdparty\pegtl\pegtl\change_action_and_states.hpp`
- `thirdparty\pegtl\pegtl\change_control.hpp`
- `thirdparty\pegtl\pegtl\change_state.hpp`
- `thirdparty\pegtl\pegtl\change_states.hpp`
- `thirdparty\pegtl\pegtl\config.hpp`
- `thirdparty\pegtl\pegtl\cstream_input.hpp`
- `thirdparty\pegtl\pegtl\demangle.hpp`
- `thirdparty\pegtl\pegtl\disable_action.hpp`
- `thirdparty\pegtl\pegtl\discard_input.hpp`
- `thirdparty\pegtl\pegtl\discard_input_on_failure.hpp`
- `thirdparty\pegtl\pegtl\discard_input_on_success.hpp`
- `thirdparty\pegtl\pegtl\enable_action.hpp`
- `thirdparty\pegtl\pegtl\eol.hpp`
- `thirdparty\pegtl\pegtl\file_input.hpp`
- `thirdparty\pegtl\pegtl\istream_input.hpp`
- `thirdparty\pegtl\pegtl\match.hpp`
- `thirdparty\pegtl\pegtl\memory_input.hpp`
- `thirdparty\pegtl\pegtl\mmap_input.hpp`
- `thirdparty\pegtl\pegtl\must_if.hpp`
- `thirdparty\pegtl\pegtl\normal.hpp`
- `thirdparty\pegtl\pegtl\nothing.hpp`
- `thirdparty\pegtl\pegtl\parse.hpp`
- `thirdparty\pegtl\pegtl\parse_error.hpp`
- `thirdparty\pegtl\pegtl\position.hpp`
- `thirdparty\pegtl\pegtl\read_input.hpp`
- `thirdparty\pegtl\pegtl\require_apply.hpp`
- `thirdparty\pegtl\pegtl\require_apply0.hpp`
- `thirdparty\pegtl\pegtl\rewind_mode.hpp`
- `thirdparty\pegtl\pegtl\rules.hpp`
- `thirdparty\pegtl\pegtl\string_input.hpp`
- `thirdparty\pegtl\pegtl\tracking_mode.hpp`
- `thirdparty\pegtl\pegtl\type_list.hpp`
- `thirdparty\pegtl\pegtl\utf8.hpp`
- `thirdparty\pegtl\pegtl\version.hpp`
- `thirdparty\pegtl\pegtl\visit.hpp`

### thirdparty\pegtl\pegtl\contrib/

- `thirdparty\pegtl\pegtl\contrib\parse_tree.hpp`
- `thirdparty\pegtl\pegtl\contrib\remove_first_state.hpp`
- `thirdparty\pegtl\pegtl\contrib\shuffle_states.hpp`

### thirdparty\pegtl\pegtl\internal/

- `thirdparty\pegtl\pegtl\internal\action.hpp`
- `thirdparty\pegtl\pegtl\internal\action_input.hpp`
- `thirdparty\pegtl\pegtl\internal\any.hpp`
- `thirdparty\pegtl\pegtl\internal\apply.hpp`
- `thirdparty\pegtl\pegtl\internal\apply0.hpp`
- `thirdparty\pegtl\pegtl\internal\apply0_single.hpp`
- `thirdparty\pegtl\pegtl\internal\apply_single.hpp`
- `thirdparty\pegtl\pegtl\internal\at.hpp`
- `thirdparty\pegtl\pegtl\internal\bof.hpp`
- `thirdparty\pegtl\pegtl\internal\bol.hpp`
- `thirdparty\pegtl\pegtl\internal\bump.hpp`
- `thirdparty\pegtl\pegtl\internal\bump_help.hpp`
- `thirdparty\pegtl\pegtl\internal\bytes.hpp`
- `thirdparty\pegtl\pegtl\internal\control.hpp`
- `thirdparty\pegtl\pegtl\internal\cr_crlf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\cr_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\crlf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\cstream_reader.hpp`
- `thirdparty\pegtl\pegtl\internal\dependent_false.hpp`
- `thirdparty\pegtl\pegtl\internal\disable.hpp`
- `thirdparty\pegtl\pegtl\internal\discard.hpp`
- `thirdparty\pegtl\pegtl\internal\enable.hpp`
- `thirdparty\pegtl\pegtl\internal\enable_control.hpp`
- `thirdparty\pegtl\pegtl\internal\eof.hpp`
- `thirdparty\pegtl\pegtl\internal\eol.hpp`
- `thirdparty\pegtl\pegtl\internal\eol_pair.hpp`
- `thirdparty\pegtl\pegtl\internal\eolf.hpp`
- `thirdparty\pegtl\pegtl\internal\failure.hpp`
- `thirdparty\pegtl\pegtl\internal\file_mapper_win32.hpp`
- `thirdparty\pegtl\pegtl\internal\file_reader.hpp`
- `thirdparty\pegtl\pegtl\internal\filesystem.hpp`
- `thirdparty\pegtl\pegtl\internal\has_apply.hpp`
- `thirdparty\pegtl\pegtl\internal\has_apply0.hpp`
- `thirdparty\pegtl\pegtl\internal\has_error_message.hpp`
- `thirdparty\pegtl\pegtl\internal\has_match.hpp`
- `thirdparty\pegtl\pegtl\internal\has_unwind.hpp`
- `thirdparty\pegtl\pegtl\internal\identifier.hpp`
- `thirdparty\pegtl\pegtl\internal\if_apply.hpp`
- `thirdparty\pegtl\pegtl\internal\if_must.hpp`
- `thirdparty\pegtl\pegtl\internal\if_must_else.hpp`
- `thirdparty\pegtl\pegtl\internal\if_then_else.hpp`
- `thirdparty\pegtl\pegtl\internal\input_pair.hpp`
- `thirdparty\pegtl\pegtl\internal\istream_reader.hpp`
- `thirdparty\pegtl\pegtl\internal\istring.hpp`
- `thirdparty\pegtl\pegtl\internal\iterator.hpp`
- `thirdparty\pegtl\pegtl\internal\lf_crlf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\lf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\list.hpp`
- `thirdparty\pegtl\pegtl\internal\list_must.hpp`
- `thirdparty\pegtl\pegtl\internal\list_tail.hpp`
- `thirdparty\pegtl\pegtl\internal\list_tail_pad.hpp`
- `thirdparty\pegtl\pegtl\internal\marker.hpp`
- `thirdparty\pegtl\pegtl\internal\minus.hpp`
- `thirdparty\pegtl\pegtl\internal\missing_apply.hpp`
- `thirdparty\pegtl\pegtl\internal\missing_apply0.hpp`
- `thirdparty\pegtl\pegtl\internal\must.hpp`
- `thirdparty\pegtl\pegtl\internal\not_at.hpp`
- `thirdparty\pegtl\pegtl\internal\one.hpp`
- `thirdparty\pegtl\pegtl\internal\opt.hpp`
- `thirdparty\pegtl\pegtl\internal\pad.hpp`
- `thirdparty\pegtl\pegtl\internal\pad_opt.hpp`
- `thirdparty\pegtl\pegtl\internal\path_to_string.hpp`
- `thirdparty\pegtl\pegtl\internal\peek_char.hpp`
- `thirdparty\pegtl\pegtl\internal\peek_utf8.hpp`
- `thirdparty\pegtl\pegtl\internal\pegtl_string.hpp`
- `thirdparty\pegtl\pegtl\internal\plus.hpp`
- `thirdparty\pegtl\pegtl\internal\raise.hpp`
- `thirdparty\pegtl\pegtl\internal\range.hpp`
- `thirdparty\pegtl\pegtl\internal\ranges.hpp`
- `thirdparty\pegtl\pegtl\internal\rematch.hpp`
- `thirdparty\pegtl\pegtl\internal\rep.hpp`
- `thirdparty\pegtl\pegtl\internal\rep_min.hpp`
- `thirdparty\pegtl\pegtl\internal\rep_min_max.hpp`
- `thirdparty\pegtl\pegtl\internal\rep_opt.hpp`
- `thirdparty\pegtl\pegtl\internal\require.hpp`
- `thirdparty\pegtl\pegtl\internal\result_on_found.hpp`
- `thirdparty\pegtl\pegtl\internal\rules.hpp`
- `thirdparty\pegtl\pegtl\internal\seq.hpp`
- `thirdparty\pegtl\pegtl\internal\sor.hpp`
- `thirdparty\pegtl\pegtl\internal\star.hpp`
- `thirdparty\pegtl\pegtl\internal\star_must.hpp`
- `thirdparty\pegtl\pegtl\internal\state.hpp`
- `thirdparty\pegtl\pegtl\internal\string.hpp`
- `thirdparty\pegtl\pegtl\internal\success.hpp`
- `thirdparty\pegtl\pegtl\internal\try_catch_type.hpp`
- `thirdparty\pegtl\pegtl\internal\until.hpp`

### thirdparty\picosha2/

- `thirdparty\picosha2\picosha2.h`

### thirdparty\potrace\include/

- `thirdparty\potrace\include\auxiliary.h`
- `thirdparty\potrace\include\bitmap.h`
- `thirdparty\potrace\include\bitmap_io.h`
- `thirdparty\potrace\include\curve.h`
- `thirdparty\potrace\include\decompose.h`
- `thirdparty\potrace\include\lists.h`
- `thirdparty\potrace\include\potrace_version.h`
- `thirdparty\potrace\include\potracelib.h`
- `thirdparty\potrace\include\progress.h`
- `thirdparty\potrace\include\trace.h`

### thirdparty\pybind11\include\pybind11/

- `thirdparty\pybind11\include\pybind11\attr.h`
- `thirdparty\pybind11\include\pybind11\buffer_info.h`
- `thirdparty\pybind11\include\pybind11\cast.h`
- `thirdparty\pybind11\include\pybind11\embed.h`
- `thirdparty\pybind11\include\pybind11\eval.h`
- `thirdparty\pybind11\include\pybind11\gil.h`
- `thirdparty\pybind11\include\pybind11\options.h`
- `thirdparty\pybind11\include\pybind11\pybind11.h`
- `thirdparty\pybind11\include\pybind11\pytypes.h`

### thirdparty\pybind11\include\pybind11\detail/

- `thirdparty\pybind11\include\pybind11\detail\class.h`
- `thirdparty\pybind11\include\pybind11\detail\common.h`
- `thirdparty\pybind11\include\pybind11\detail\descr.h`
- `thirdparty\pybind11\include\pybind11\detail\init.h`
- `thirdparty\pybind11\include\pybind11\detail\internals.h`
- `thirdparty\pybind11\include\pybind11\detail\type_caster_base.h`
- `thirdparty\pybind11\include\pybind11\detail\typeid.h`

### thirdparty\rectpack2d\rectpack2d/

- `thirdparty\rectpack2d\rectpack2d\best_bin_finder.h`
- `thirdparty\rectpack2d\rectpack2d\empty_space_allocators.h`
- `thirdparty\rectpack2d\rectpack2d\empty_spaces.h`
- `thirdparty\rectpack2d\rectpack2d\finders_interface.h`
- `thirdparty\rectpack2d\rectpack2d\insert_and_split.h`
- `thirdparty\rectpack2d\rectpack2d\rect_structs.h`

### thirdparty\rtree\geometry/

- `thirdparty\rtree\geometry\rtree.h`

### thirdparty\thread-pool/

- `thirdparty\thread-pool\bs_thread_pool.hpp`

### thirdparty\tinyspline_lib/

- `thirdparty\tinyspline_lib\parson.h`
- `thirdparty\tinyspline_lib\tinyspline.h`
- `thirdparty\tinyspline_lib\tinysplinecxx.h`

### utils\idftools/

- `utils\idftools\idf_common.h`
- `utils\idftools\idf_helpers.h`
- `utils\idftools\idf_outlines.h`
- `utils\idftools\idf_parser.h`
- `utils\idftools\vrml_layer.h`

### 外部路径/

- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_bit_utils.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_chrono.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_filebuf.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_format_ucd_tables.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_formatter.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_heap_algorithms.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_int128.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_iter_core.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_minmax.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_ostream.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_print.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_ranges_to.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_ranges_tuple_formatter.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_sanitizer_annotate_container.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_string_view.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_system_error_abi.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_threads_core.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_tzdb.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_xlocinfo_types.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ammintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\concurrencysal.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\crtdefs.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\eh.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\emmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\excpt.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\immintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\intrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\intrin0.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\intrin0.inl.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\iso646.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\limits.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\mmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\nmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pplcancellation_token.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pplinterface.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ppltasks.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ppltaskscheduler.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pplwin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\sal.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\setjmp.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\smmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\stdarg.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\stdbool.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\stdint.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\tmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\use_ansi.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vadefs.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_exception.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_new.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_new_debug.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_startup.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_string.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_typeinfo.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\wmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xatomic.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xatomic_wait.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xbit_ops.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcall_once.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv_ryu.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv_ryu_tables.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv_tables.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xerrc.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xfilesystem_abi.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xkeycheck.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xnode_handle.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xpolymorphic_allocator.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xsmf_control.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xthreads.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xtimec.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ymath.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\yvals.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\yvals_core.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\zmmintrin.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\WTypesbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\apiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\apisetcconv.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\basetsd.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\bcrypt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\driverspecs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\guiddef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\in6addr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\inaddr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\kernelspecs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\ktmtypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\minwindef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\poppack.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack1.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack4.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack8.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\qos.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcasync.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcdce.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcdcep.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcndr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcnterr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcsal.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\sdkddkver.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\sdv_driverspecs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\specstrings.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\specstrings_strict.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\specstrings_undef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\stralign.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\strsafe.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\tvout.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\winapifamily.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\windef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\winerror.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\winpackagefamily.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\wnnc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\ws2ipdef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\wtypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\assert.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_io.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_malloc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_math.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_math_defines.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_memcpy_s.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_memory.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_search.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_share.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_startup.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_stdio_config.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_terminate.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wconio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wctype.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wdirect.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wprocess.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wstdio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wstdlib.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wstring.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wtime.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\crtdbg.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\ctype.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\direct.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\dos.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\errno.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\fcntl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\float.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\inttypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\io.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\locale.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\malloc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\math.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\memory.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\process.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\share.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\stddef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\stdio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\stdlib.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\string.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\sys\stat.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\sys\types.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\tchar.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\time.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\wchar.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\wctype.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\CommCtrl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\DocObj.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ExDisp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\IsGuids.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\KnownFolders.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ObjectArray.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\PropIdl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\PropIdlBase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShObjIdl_core.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShlDisp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShlGuid.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShlObj_core.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Shlwapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\SoftPub.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\StructuredQueryCondition.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Unknwn.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Unknwnbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\VersionHelpers.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WS2tcpip.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WerApi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinBase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinNls.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinSock2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinTrust.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinUser.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Windows.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\apiquery2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\cguid.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\combaseapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\comcat.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\coml2api.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\consoleapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\consoleapi2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\consoleapi3.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\datetimeapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\debugapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\dpa_dsa.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\dpapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\enclaveapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\errhandlingapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\fibersapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\fileapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\fileapifromapp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplus.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusbitmap.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusbrush.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluscachedbitmap.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluscolor.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluscolormatrix.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusenums.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusflat.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusfont.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusfontcollection.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusfontfamily.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusgpstubs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusgraphics.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusheaders.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusimageattributes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusimagecodec.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusimaging.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusinit.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluslinecaps.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusmatrix.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusmem.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusmetafile.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusmetaheader.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluspath.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluspen.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdipluspixelformats.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusregion.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplusstringformat.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\gdiplustypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\handleapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\heapapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ime_cmodes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\imm.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\interlockedapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ioapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\jobapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\jobapi2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\libloaderapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\mcx.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\memoryapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\minwinbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\msxml.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\namedpipeapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\namespaceapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ncrypt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\oaidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\objbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\objidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\objidlbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ocidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ole2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\oleauto.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\oleidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\processenv.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\processthreadsapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\processtopologyapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\profileapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propkey.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propkeydef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propsys.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propvarutil.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\prsht.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\realtimeapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\reason.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\rpcnsi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\rpcnsip.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\securityappcontainer.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\securitybaseapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\servprov.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\shellapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sherrors.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\shtypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sql.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sqlext.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sqltypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sqlucode.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\stringapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\synchapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sysinfoapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\systemtopologyapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\threadpoolapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\threadpoollegacyapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\timezoneapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\urlmon.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\utilapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\verrsrc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wincon.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wincontypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wincrypt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wingdi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winhttp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winnetwk.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winnt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winreg.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winsvc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winver.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wow64apiset.h`

## ⚠️ 未解析符号 (外部依赖)

以下符号需要通过外部库或vcpkg安装包提供：

### Windows/MSVC运行时

- `??_7type_info@@6B@`
- `??_E?$DIALOG_SIM_MODEL@VLIB_SYMBOL@@@@UEAAPEAXI@Z`
- `??_E?$DIALOG_SIM_MODEL@VSCH_SYMBOL@@@@UEAAPEAXI@Z`
- `??_E?$DataHolder@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$wxAnyValueTypeOpsGeneric@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@wxPrivate@@UEAAPEAXI@Z`
- `??_E?$DataHolder@VCOLOR4D@KIGFX@@@?$wxAnyValueTypeOpsGeneric@VCOLOR4D@KIGFX@@@wxPrivate@@UEAAPEAXI@Z`
- `??_E?$DataHolder@VSHAPE_LINE_CHAIN@@@?$wxAnyValueTypeOpsGeneric@VSHAPE_LINE_CHAIN@@@wxPrivate@@UEAAPEAXI@Z`
- `??_E?$DataHolder@VwxDataViewIconText@@@?$wxAnyValueTypeOpsGeneric@VwxDataViewIconText@@@wxPrivate@@UEAAPEAXI@Z`
- `??_E?$EDIT_CONSTRAINT@VEDIT_LINE@@@@UEAAPEAXI@Z`
- `??_E?$EDIT_CONSTRAINT@VEDIT_POINT@@@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@W4PCB_LAYER_ID@@P8BOARD_ITEM@@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@W4PCB_LAYER_ID@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_ITEM@@W4KICAD_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@VCOLOR4D@KIGFX@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@VEDA_ANGLE@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@W4LINE_STYLE@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@W4SHAPE_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@VCOLOR4D@KIGFX@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@VwxString@@P81@EBAAEBV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@W4GR_TEXT_H_ALIGN_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@W4GR_TEXT_V_ALIGN_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@V?$optional@H@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@V?$optional@N@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@W4PCB_LAYER_ID@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@W4ZONE_CONNECTION@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@V?$optional@H@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@V?$optional@N@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@VwxString@@P81@EBAAEBV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_ATTRIB@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_DRILL_SHAPE@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_PROP@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_SHAPE@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4UNCONNECTED_LAYER_MODE@PADSTACK@@P81@EBA?AW423@XZ@@UEAAPEAXI@Z`
- ... 还有 358 个符号

### 其他

- `??0?$RepeatedField@H@protobuf@google@@QEAA@AEBV012@@Z`
- `??0?$RepeatedField@H@protobuf@google@@QEAA@PEAVArena@12@@Z`
- `??0?$RepeatedPtrField@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@protobuf@google@@QEAA@AEBV012@@Z`
- `??0?$RepeatedPtrField@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@protobuf@google@@QEAA@PEAVArena@12@@Z`
- `??0APIHeaderSection_MakeHeader@@QEAA@AEBV?$handle@VStepData_StepModel@@@opencascade@@@Z`
- `??0APIHeaderSection_MakeHeader@@QEAA@H@Z`
- `??0BRepAdaptor_Surface@@QEAA@AEBVTopoDS_Face@@_N@Z`
- `??0BRepAlgoAPI_Check@@QEAA@AEBVTopoDS_Shape@@_N1AEBVMessage_ProgressRange@@@Z`
- `??0BRepAlgoAPI_Cut@@QEAA@XZ`
- `??0BRepAlgoAPI_Fuse@@QEAA@XZ`
- `??0BRepBuilderAPI_GTransform@@QEAA@AEBVgp_GTrsf@@@Z`
- `??0BRepBuilderAPI_MakeEdge@@QEAA@AEBV?$handle@VGeom_Curve@@@opencascade@@@Z`
- `??0BRepBuilderAPI_MakeEdge@@QEAA@AEBVgp_Pnt@@0@Z`
- `??0BRepBuilderAPI_MakeFace@@QEAA@AEBVgp_Pln@@AEBVTopoDS_Wire@@_N@Z`
- `??0BRepBuilderAPI_MakeFace@@QEAA@XZ`
- `??0BRepBuilderAPI_MakeWire@@QEAA@XZ`
- `??0BRepExtrema_DistShapeShape@@QEAA@AEBVTopoDS_Shape@@0W4Extrema_ExtFlag@@W4Extrema_ExtAlgo@@AEBVMessage_ProgressRange@@@Z`
- `??0BRepLib_MakeWire@@QEAA@XZ`
- `??0BRepMesh_IncrementalMesh@@QEAA@AEBVTopoDS_Shape@@N_NN1@Z`
- `??0BRepPrimAPI_MakePrism@@QEAA@AEBVTopoDS_Shape@@AEBVgp_Vec@@_N2@Z`
- `??0BRepTools_Modifier@@QEAA@_N@Z`
- `??0Bnd_BoundSortBox@@QEAA@XZ`
- `??0Bnd_Box@@QEAA@XZ`
- `??0GC_MakeArcOfCircle@@QEAA@AEBVgp_Pnt@@00@Z`
- `??0GC_MakeCircle@@QEAA@AEBVgp_Ax2@@N@Z`
- `??0GC_MakeCircle@@QEAA@AEBVgp_Pnt@@00@Z`
- `??0GProp_GProps@@QEAA@XZ`
- `??0IGESControl_Controller@@QEAA@_N@Z`
- `??0IGESControl_Reader@@QEAA@XZ`
- `??0Message_Level@@QEAA@AEBVTCollection_AsciiString@@@Z`
- `??0Message_Printer@@IEAA@XZ`
- `??0Message_PrinterOStream@@QEAA@W4Message_Gravity@@@Z`
- `??0NCollection_IncAllocator@@QEAA@_K@Z`
- `??0OSD_MemInfo@@QEAA@_N@Z`
- `??0Quantity_Color@@QEAA@NNNW4Quantity_TypeOfColor@@@Z`
- `??0RWGltf_CafWriter@@QEAA@AEBVTCollection_AsciiString@@_N@Z`
- `??0RWMesh_CafReader@@QEAA@XZ`
- `??0RWMesh_CoordinateSystemConverter@@QEAA@XZ`
- `??0RWPly_CafWriter@@QEAA@AEBVTCollection_AsciiString@@@Z`
- `??0STEPCAFControl_Reader@@QEAA@XZ`
- `??0STEPCAFControl_Writer@@QEAA@XZ`
- `??0ShapeUpgrade_UnifySameDomain@@QEAA@AEBVTopoDS_Shape@@_N11@Z`
- `??0Standard_Failure@@QEAA@AEBV0@@Z`
- `??0Standard_Failure@@QEAA@QEBD@Z`
- `??0Standard_OutOfMemory@@QEAA@QEBD@Z`
- `??0StlAPI_Writer@@QEAA@XZ`
- `??0TCollection_AsciiString@@QEAA@$$QEAV0@@Z`
- `??0TCollection_AsciiString@@QEAA@AEBVTCollection_ExtendedString@@D@Z`
- `??0TCollection_AsciiString@@QEAA@QEBD@Z`
- `??0TCollection_AsciiString@@QEAA@XZ`
- ... 还有 42 个符号

**注意**: 总共有 13538 个未解析符号，此处仅显示前500个

## 🚀 使用建议

### 1. 源文件复制
```bash
# 将最小源文件集合复制到目标项目
# 建议保持相对目录结构
```

### 2. 头文件复制
```bash
# 将头文件闭包复制到目标项目
# 注意保持include路径的一致性
```

### 3. 外部依赖安装
通过vcpkg安装以下包来解决外部符号依赖：
- wxwidgets (如果使用Qt替代，可能不需要)
- boost
- openssl
- sqlite3
- cairo
- 其他根据未解析符号确定的包

## ⚠️ 重要提醒

1. **编译验证**: 此分析基于静态依赖，复制后需要验证编译通过
2. **功能测试**: 最小集合可能缺少运行时动态加载的组件
3. **增量添加**: 如果功能不完整，可以以此为基础逐步添加缺失部分
4. **版本兼容**: 确保外部库版本与KiCad编译环境兼容

---
**报告生成时间**: 1756558555.6183507
**工具**: KiCad依赖分析工具链 (clang-scan-deps + 符号分析)
**目标**: KiCad Qt移植项目最小依赖集合