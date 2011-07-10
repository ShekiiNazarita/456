var new_message_count = 0;

function watchers(count, names)
{
    $("#spectator_count").html(count + " spectators");
    $("#spectator_list").html(names);
}

function chat(msg)
{
    $("#chat_history").append(msg + "<br>");
    $("#chat_history_container").scrollTop($("#chat_history").height());
    if ($("#chat_body").css("display") === "none")
    {
        new_message_count++;
        update_message_count();
    }
}

function update_message_count()
{
    var press_ = new_message_count > 0 ? " (Press _)" : "";
    $("#message_count").html(new_message_count + " new messages" + press_);
    $("#message_count").toggleClass("has_new", new_message_count > 0);
}

function chat_message_send(e)
{
    if (e.which == 13)
    {
        e.preventDefault();
        e.stopPropagation();
        socket.send("Chat: " + $("#chat_input").val());
        $("#chat_input").val("");
        return false;
    }
    else if (e.which == 27)
    {
        e.preventDefault();
        e.stopPropagation();
        toggle_chat();
        $(document.activeElement).blur();
    }
    return true;
}

function toggle_chat()
{
    if ($("#chat_body").css("display") === "none")
    {
        $("#chat_body").slideDown(200);
        new_message_count = 0;
        update_message_count();
        $("#message_count").html("(Esc: back to game)");
    }
    else
    {
        $("#chat_body").slideUp(200);
        update_message_count();
    }
}

function focus_chat()
{
    if ($("#chat_body").css("display") === "none")
        toggle_chat();

    $("#chat_history_container").scrollTop($("#chat_history").height());
    $("#chat_input").focus();
}

function clear_chat()
{
    $("#spectator_list").html("&nbsp;");
    $("#chat_history").html("");
    $("#spectator_count").html("0 spectators");
    new_message_count = 0;
    update_message_count();
    $("#chat_body").slideUp(200);
}
