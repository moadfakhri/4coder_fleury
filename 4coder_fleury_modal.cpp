// NOTE(fakhri): the modes gonna be similar to vim modes
//~

//[x] Normal mode 
//[x] Insert mode
//[!] Delete mode
//[!] Change mode
//[x] Command mode
//[x] search mode
//[!] Goto mode
//[!] Yank mode
//[x] comment mode

//~ Modes settings
global String_ID modal_mode_map_id;
global String_ID modal_normal_map_id ;
global String_ID modal_insert_map_id;
global String_ID modal_delete_map_id;
global String_ID modal_yank_map_id;
global String_ID modal_change_map_id;
global String_ID modal_goto_map_id;
global String_ID modal_search_map_id;
global String_ID modal_comment_map_id;

// NOTE(fakhri): 
//
//casey_newline_and_indent
//casey_delete_to_end_of_line
//f4_go_to_definition
//f4_go_to_definition_same_panel
//

//~ NOTE(fakhri): Helper functions

// TODO(fakhri): vim count thing
// TODO(fakhri): Vim dot command so we don't repeat ourselves so much

function Command_Map_ID*
get_current_mapid_ptr(Application_Links *app) {
    View_ID view = get_active_view( app, 0 );
    Buffer_ID buffer = view_get_buffer( app, view, 0 );
    Managed_Scope scope = buffer_get_managed_scope( app, buffer );
    Command_Map_ID *result_ptr = scope_attachment( app, scope, buffer_map_id, Command_Map_ID );
    return result_ptr;
}

function void 
set_current_mapid(Application_Links *app, Command_Map_ID mapid ) {
    Command_Map_ID *map_id_ptr = get_current_mapid_ptr(app);
    if (map_id_ptr != 0)
    {
        *map_id_ptr = mapid;
    }
}

inline 
function b32 
modal_active_buffer_has_write_access(Application_Links *app) {
    View_ID view = get_active_view(app, Access_Write);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Write);
    b32 result = (buffer_get_access_flags(app, buffer) & Access_Write);
    return result;
}


inline
function String_Const_u8
modal_get_mode_as_string(String_ID mode_mapid)
{
    String_Const_u8 result = {};
    
    if      (mode_mapid == modal_normal_map_id)  result = string_u8_litexpr("| NORMAL | ");
    else if (mode_mapid == modal_insert_map_id)  result = string_u8_litexpr("| INSERT | ");
    else if (mode_mapid == modal_delete_map_id)  result = string_u8_litexpr("| DELETE | ");
    else if (mode_mapid == modal_yank_map_id)    result = string_u8_litexpr("|  YANK  | ");
    else if (mode_mapid == modal_change_map_id)  result = string_u8_litexpr("| CHANGE | ");
    else if (mode_mapid == modal_goto_map_id)    result = string_u8_litexpr("|  GOTO  | ");
    else if (mode_mapid == modal_search_map_id)  result = string_u8_litexpr("| SEARCH | ");
    else if (mode_mapid == modal_comment_map_id) result = string_u8_litexpr("|COMMENTS| ");
    
    return result; 
}

inline function b32
is_insert_mode(Application_Links *app)
{
    String_ID current_mapid = (String_ID)*get_current_mapid_ptr(app);
    return (current_mapid == modal_insert_map_id);
}

//~ NOTE(fakhri): Vim like commands
CUSTOM_COMMAND_SIG(w)
CUSTOM_DOC("save the current buffer.") {
    save(app);
}

CUSTOM_COMMAND_SIG(wa)
CUSTOM_DOC("save all dirty buffers.") {
    save_all_dirty_buffers(app);
}

CUSTOM_COMMAND_SIG(q)
CUSTOM_DOC("close the current panel.") {
    close_panel(app);
}

CUSTOM_COMMAND_SIG(wq)
CUSTOM_DOC("write the current buffer and close the current panel.") {
    w(app);
    q(app);
}

CUSTOM_COMMAND_SIG(qa)
CUSTOM_DOC("close all the panel.") {
    exit_4coder(app);
}

CUSTOM_COMMAND_SIG(wqa)
CUSTOM_DOC("write all dirty buffers and close all the panels.") {
    wa(app);
    qa(app);
}

CUSTOM_COMMAND_SIG(b)
CUSTOM_DOC("list all the buffers.") {
    interactive_switch_buffer(app);
}

CUSTOM_COMMAND_SIG(kb)
CUSTOM_DOC("delete the current buffer.") {
    kill_buffer(app);
}

CUSTOM_COMMAND_SIG(kbi)
CUSTOM_DOC("kill buffers interactively.") {
    interactive_kill_buffer(app);
}

CUSTOM_COMMAND_SIG(e)
CUSTOM_DOC("openinteractive_open_or_new a file.") {
    interactive_open_or_new(app);
}

//~ NOTE(fakhri): normal mode
CUSTOM_COMMAND_SIG(modal_set_mode_normal)
CUSTOM_DOC("activates 'normal' mode.") {
    set_current_mapid(app, modal_normal_map_id);
}

//~ NOTE(fakhri): insert mode
CUSTOM_COMMAND_SIG(modal_set_mode_insert)
CUSTOM_DOC("activates 'insert' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        set_current_mapid(app, modal_insert_map_id);
    }
}

CUSTOM_COMMAND_SIG(modal_set_mode_insert_above)
CUSTOM_DOC("insert new line above the current line and activates 'insert' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        seek_beginning_of_line(app);
        write_text(app, string_u8_litexpr("\n"));
        move_left(app);
        auto_indent_line_at_cursor(app);
        modal_set_mode_insert(app);
    }
}

CUSTOM_COMMAND_SIG(modal_set_mode_insert_below)
CUSTOM_DOC("insert new line below the current line and activates 'insert' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        seek_end_of_line(app);
        casey_newline_and_indent(app);
        modal_set_mode_insert(app);
    }
}

CUSTOM_COMMAND_SIG(modal_set_mode_insert_append)
CUSTOM_DOC("move cursor to the right and activates 'insert' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        move_right(app);
        modal_set_mode_insert(app);
    }
}

CUSTOM_COMMAND_SIG(modal_set_mode_insert_seek_eof)
CUSTOM_DOC("move cursor to the end of line and activates 'insert' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        seek_end_of_textual_line(app);
        modal_set_mode_insert(app);
    }
}

//~ NOTE(fakhri): yank mode
CUSTOM_COMMAND_SIG(modal_set_mode_yank)
CUSTOM_DOC("activates 'yank' mode.") {
    set_current_mapid(app, modal_yank_map_id);
}

CUSTOM_COMMAND_SIG(copy_selection_and_switch_to_normal_mode)
CUSTOM_DOC("copy region between the mark and the cursor and then switch back to normal mode.") {
    copy(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(copy_line)
{
    seek_beginning_of_line(app);
    move_left(app);
    set_mark(app);
    seek_end_of_line(app);
    copy(app);
}

CUSTOM_COMMAND_SIG(copy_line_and_switch_to_normal_mode)
CUSTOM_DOC("copy the line under the cursor and then switch back to normal mode.") {
    copy_line(app);
    modal_set_mode_normal(app);
}

//~ NOTE(fakhri): delete mode
CUSTOM_COMMAND_SIG(modal_set_mode_delete)
CUSTOM_DOC("activates 'delete' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        set_current_mapid(app, modal_delete_map_id);
    }
}

CUSTOM_COMMAND_SIG(delete_line_and_switch_to_normal_mode)
CUSTOM_DOC("delete the lin under the cursor and then switch back to normal mode.") {
    copy_line(app);
    move_right(app);
    delete_line(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(delete_alpha_numeric_boundary_and_switch_to_normal_mode)
CUSTOM_DOC("delete the lin under the cursor and then switch back to normal mode.") {
    delete_alpha_numeric_boundary(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(delete_range_and_switch_to_normal_mode)
CUSTOM_DOC("delete the lin under the cursor and then switch back to normal mode.") {
    copy(app);
    delete_range(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(delete_to_end_of_line_and_switch_to_normal_mode)
CUSTOM_DOC("delete the lin under the cursor and then switch back to normal mode.") {
    casey_delete_to_end_of_line(app);
    modal_set_mode_normal(app);
}

//~ NOTE(fakhri): change mode
CUSTOM_COMMAND_SIG(modal_set_mode_change)
CUSTOM_DOC("activates 'change' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        set_current_mapid(app, modal_change_map_id);
    }
}

CUSTOM_COMMAND_SIG(change_line)
CUSTOM_DOC("delete the current line and change to insert mode ") {
    delete_line(app);
    modal_set_mode_insert(app);
}

CUSTOM_COMMAND_SIG(change_alpha_numeric_boundary)
CUSTOM_DOC("delete the word starting at the cursor position and change to insert mode.") {
    delete_alpha_numeric_boundary(app);
    modal_set_mode_insert(app);
}

CUSTOM_COMMAND_SIG(change_range)
CUSTOM_DOC("delete the range between the mark and cursor and change to insert mode.") {
    delete_range(app);
    modal_set_mode_insert(app);
}

CUSTOM_COMMAND_SIG(change_to_end_of_line)
CUSTOM_DOC("delete text between the cursor and the end of line and change to insert mode.") {
    casey_delete_to_end_of_line(app);
    modal_set_mode_insert(app);
}

//~ NOTE(fakhri): search mode
CUSTOM_COMMAND_SIG(modal_set_mode_search)
CUSTOM_DOC("activates 'search' mode.") {
    set_current_mapid(app, modal_search_map_id);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_list_all_locations)
CUSTOM_DOC("change to normal mode and then go to line number.") {
    modal_set_mode_normal(app);
    list_all_locations(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_list_all_locations_of_identifier)
CUSTOM_DOC("change to normal mode and then go to line number.") {
    modal_set_mode_normal(app);
    list_all_locations_of_identifier(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_list_all_functions_all_buffers)
CUSTOM_DOC("change to normal mode and then go to line number.") {
    modal_set_mode_normal(app);
    list_all_functions_all_buffers(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_list_all_locations_of_selection)
CUSTOM_DOC("change to normal mode and then go to line number.") {
    modal_set_mode_normal(app);
    list_all_locations_of_selection(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_list_all_locations_of_type_definition_of_identifier)
CUSTOM_DOC("change to normal mode and then go to line number.") {
    modal_set_mode_normal(app);
    list_all_locations_of_type_definition_of_identifier(app);
}

//~ NOTE(fakhri): goto mode
CUSTOM_COMMAND_SIG(modal_set_mode_goto)
CUSTOM_DOC("activates 'Goto' mode.") {
    set_current_mapid(app, modal_goto_map_id);
}

CUSTOM_COMMAND_SIG(goto_beginning_of_file_and_switch_to_normal_mode)
CUSTOM_DOC("go to the beginning of the file and the end of line and change to normal mode.") {
    goto_beginning_of_file(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_goto_line)
CUSTOM_DOC("change to normal mode and then go to line number.") {
    modal_set_mode_normal(app);
    goto_line(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_goto_definition)
CUSTOM_DOC("change to normal mode and then go to definition.") {
    modal_set_mode_normal(app);
    f4_go_to_definition(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_goto_definition_same_panel)
CUSTOM_DOC("change to normal mode and then go to definition in the same panel.") {
    modal_set_mode_normal(app);
    f4_go_to_definition_same_panel(app);
}

CUSTOM_COMMAND_SIG(switch_to_normal_mode_and_open_file_in_quotes)
CUSTOM_DOC("change to normal mode and then open the file matching the one inside the quotes surrounding the cursor (if any).") {
    modal_set_mode_normal(app);
    open_file_in_quotes(app);
}

//~ NOTE(fakhri): comment mode
CUSTOM_COMMAND_SIG(modal_set_mode_comment)
CUSTOM_DOC("activates 'Comment' mode.") {
    if (modal_active_buffer_has_write_access(app))
    {
        set_current_mapid(app, modal_comment_map_id);
    }
}

CUSTOM_COMMAND_SIG(comment_line_toggle_and_switch_to_normal_mode)
CUSTOM_DOC("toggle the current line comment.") {
    comment_line_toggle(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(comment_selection_and_switch_to_normal_mode)
CUSTOM_DOC("comment the selected text and then switch back to normal mode.") {
    f4_comment_selection(app);
    modal_set_mode_normal(app);
}

CUSTOM_COMMAND_SIG(uncomment_selection_and_switch_to_normal_mode)
CUSTOM_DOC("uncomment the selected text and then switch back to normal mode") {
    f4_uncomment_selection(app);
    modal_set_mode_normal(app);
}



//~ NOTE(fakhri): setup Bindings
function void
F4_set_Normal_mode_bindings(Mapping *mapping)
{
    String_ID global_map_id = vars_save_string_lit("keys_global");
    modal_normal_map_id = vars_save_string_lit("normal_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_normal_map_id);
    ParentMap(global_map_id);
    
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    BindMouseMove(click_set_cursor_if_lbutton);
    
    Bind(move_right,                     KeyCode_L);
    Bind(move_left,                      KeyCode_H);
    Bind(move_up,                        KeyCode_K);
    Bind(move_down,                      KeyCode_J);
    
    Bind(move_left_alpha_numeric_boundary,       KeyCode_B);
    Bind(move_right_alpha_numeric_boundary,      KeyCode_E);
    Bind(move_right_alpha_numeric_boundary,      KeyCode_W);
    
    Bind(seek_end_of_line,               KeyCode_L, KeyCode_Shift);
    Bind(fleury_home,                    KeyCode_H, KeyCode_Shift);
    
    Bind(move_up_to_blank_line_end,      KeyCode_K, KeyCode_Shift);
    Bind(move_down_to_blank_line_end,    KeyCode_J, KeyCode_Shift);
    
    Bind(goto_end_of_file,               KeyCode_G, KeyCode_Shift);
    Bind(delete_char,                    KeyCode_X);
    Bind(undo,                           KeyCode_U);
    Bind(redo,                           KeyCode_R, KeyCode_Shift);
    Bind(paste,                          KeyCode_P);
    
    Bind(set_mark,                       KeyCode_Space);
    Bind(cursor_mark_swap,               KeyCode_Space, KeyCode_Shift);
    Bind(setup_new_project,              KeyCode_BackwardSlash);
    
    Bind(if_read_only_goto_position,            KeyCode_Return);
    Bind(if_read_only_goto_position_same_panel, KeyCode_Return, KeyCode_Shift);
    Bind(change_active_panel, KeyCode_Tab);
    Bind(swap_panels, KeyCode_Tab, KeyCode_Shift);
    //Bind(view_jump_list_with_lister,  KeyCode_Period, KeyCode_Control, KeyCode_Shift);
    
    // NOTE(fakhri): transition to other modes
    Bind(command_lister,                 KeyCode_Semicolon, KeyCode_Shift);
    Bind(modal_set_mode_insert,          KeyCode_I);
    Bind(modal_set_mode_insert_seek_eof, KeyCode_I, KeyCode_Shift);
    Bind(modal_set_mode_insert_below,    KeyCode_O);
    Bind(modal_set_mode_insert_above,    KeyCode_O, KeyCode_Shift);
    Bind(modal_set_mode_insert_append,   KeyCode_A);
    Bind(modal_set_mode_delete,          KeyCode_D);
    Bind(modal_set_mode_yank,            KeyCode_Y);
    Bind(modal_set_mode_change,          KeyCode_C);
    Bind(modal_set_mode_goto,            KeyCode_G);
    Bind(modal_set_mode_search,          KeyCode_S);
    Bind(search,                         KeyCode_ForwardSlash);
    Bind(search_identifier,              KeyCode_ForwardSlash, KeyCode_Shift);
    Bind(modal_set_mode_comment,         KeyCode_Semicolon);
}


function void
F4_set_Insert_mode_bindings(Mapping *mapping)
{
    String_ID parent_map_id = vars_save_string_lit("keys_code");
    modal_insert_map_id = vars_save_string_lit("insert_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_insert_map_id);
    ParentMap(parent_map_id);
    
    Bind(modal_set_mode_normal, KeyCode_Escape);
}


function void
F4_set_Delete_mode_bindings(Mapping *mapping)
{
    modal_delete_map_id = vars_save_string_lit("delete_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_delete_map_id);
    ParentMap(modal_mode_map_id);
    
    Bind(delete_line_and_switch_to_normal_mode, KeyCode_D);
    Bind(delete_alpha_numeric_boundary_and_switch_to_normal_mode, KeyCode_W);
    Bind(delete_range_and_switch_to_normal_mode, KeyCode_R);
    Bind(delete_to_end_of_line_and_switch_to_normal_mode, KeyCode_L);
}

function void
F4_set_Change_mode_bindings(Mapping *mapping)
{
    modal_change_map_id = vars_save_string_lit("change_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_change_map_id);
    ParentMap(modal_mode_map_id);
    
    Bind(change_line, KeyCode_C);
    Bind(change_alpha_numeric_boundary, KeyCode_W);
    Bind(change_range, KeyCode_R);
    Bind(change_to_end_of_line, KeyCode_L);
}

function void
F4_set_Yank_mode_bindings(Mapping *mapping)
{
    modal_yank_map_id = vars_save_string_lit("yank_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_yank_map_id);
    ParentMap(modal_mode_map_id);
    
    // copy range
    Bind(copy_selection_and_switch_to_normal_mode, KeyCode_R);
    Bind(copy_line_and_switch_to_normal_mode,      KeyCode_Y);
    // TODO(fakhri): yanking current line, yank word, yank find, and the inner and arround yanks...
}

function void
F4_set_Goto_mode_bindings(Mapping *mapping)
{
    modal_goto_map_id = vars_save_string_lit("goto_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_goto_map_id);
    ParentMap(modal_mode_map_id);
    
    Bind(goto_beginning_of_file_and_switch_to_normal_mode,     KeyCode_G);
    Bind(switch_to_normal_mode_and_goto_line,                  KeyCode_N);
    Bind(switch_to_normal_mode_and_goto_definition,            KeyCode_D);
    Bind(switch_to_normal_mode_and_goto_definition_same_panel, KeyCode_D, KeyCode_Shift);
    Bind(switch_to_normal_mode_and_open_file_in_quotes,        KeyCode_F);
    
    // TODO(fakhri): implement yank current line, yank word, yank find, and the inner and arround yanks
}

function void
F4_set_Search_mode_bindings(Mapping *mapping)
{
    modal_search_map_id = vars_save_string_lit("search_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_search_map_id);
    ParentMap(modal_mode_map_id);
    
    Bind(switch_to_normal_mode_and_list_all_locations,                                  KeyCode_L);
    Bind(switch_to_normal_mode_and_list_all_locations_of_identifier,                    KeyCode_I);
    Bind(switch_to_normal_mode_and_list_all_functions_all_buffers,                      KeyCode_F);
    Bind(switch_to_normal_mode_and_list_all_locations_of_selection,                     KeyCode_R);
    Bind(switch_to_normal_mode_and_list_all_locations_of_type_definition_of_identifier, KeyCode_T);
}

function void
F4_set_Comment_mode_bindings(Mapping *mapping)
{
    modal_comment_map_id = vars_save_string_lit("comment_mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_comment_map_id);
    ParentMap(modal_mode_map_id);
    
    Bind(comment_line_toggle_and_switch_to_normal_mode, KeyCode_Semicolon);
    Bind(comment_selection_and_switch_to_normal_mode,   KeyCode_C);
    Bind(uncomment_selection_and_switch_to_normal_mode, KeyCode_U);
    
}

//~ NOTE(fakhri): call this function to set up modal bindings
function void
F4_set_modal_bindings(Mapping *mapping)
{
    String_ID parent_map_id = vars_save_string_lit("keys_global");
    modal_mode_map_id = vars_save_string_lit("mode");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(modal_mode_map_id);
    ParentMap(parent_map_id);
    
    Bind(modal_set_mode_normal, KeyCode_Escape);
    
    F4_set_Normal_mode_bindings(mapping);
    F4_set_Goto_mode_bindings(mapping);
    F4_set_Search_mode_bindings(mapping);
    F4_set_Yank_mode_bindings(mapping);
    F4_set_Change_mode_bindings(mapping);
    F4_set_Delete_mode_bindings(mapping);
    F4_set_Insert_mode_bindings(mapping);
    F4_set_Comment_mode_bindings(mapping);
}