import { Component ,OnDestroy,AfterViewInit } from '@angular/core';
import * as createjs from 'createjs-module';
import { Subscription }  from "rxjs";
import {McuCommand,McuFeedback} from "../_models/index"
import {WebmqttService} from "./webmqtt.service"


@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})



export class AppComponent  {
  
}
